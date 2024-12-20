#include "../context.h"
#include "../error.h"
#include "../state.h"
#include "worker.h"
#include <ctime>

namespace redasm {

namespace {

// clang-format off
enum WorkerSteps {
    WS_INIT = 0,
    WS_EMULATE1, STEP_ANALYZE1, // First pass
    STEP_MERGECODE,
    STEP_EMULATE2, STEP_ANALYZE2, // Second pass
    STEP_MERGEDATA,
    STEP_FINALIZE,
    STEP_DONE,
    STEP_COUNT,
};
// clang-format on

constexpr std::array<const char*, STEP_COUNT> STEP_NAMES = {
    "Init",
    "Emulate #1",
    "Analyze #1",
    "Merge Code",
    "Emulate #2",
    "Analyze #2"
    "Merge Data"
    "Finalize",
    "Done",
};

} // namespace

Worker::Worker(): m_currentstep{WS_INIT} {
    m_status = std::make_unique<RDWorkerStatus>();
}

bool Worker::execute(const RDWorkerStatus** s) {
    m_status->busy = m_currentstep < STEP_DONE;
    m_status->currentstep = STEP_NAMES.at(m_currentstep);
    m_status->address.valid = false;
    m_status->listingchanged = false;

    if(m_status->busy) {
        switch(m_currentstep) {
            case WS_INIT: this->init_step(); break;

            case WS_EMULATE1:
            case STEP_EMULATE2: this->emulate_step(); break;

            case STEP_ANALYZE1:
            case STEP_ANALYZE2: this->analyze_step(); break;

            case STEP_MERGECODE: this->mergecode_step(); break;
            case STEP_MERGEDATA: this->mergedata_step(); break;
            case STEP_FINALIZE: this->finalize_step(); break;
            default: unreachable;
        }
    }
    else {
        state::context->process_listing();
        m_status->listingchanged = true;
    }

    if(s)
        *s = m_status.get();

    return m_status->busy;
}

void Worker::execute(usize step) {
    if(step == m_currentstep)
        return;

    m_currentstep = step;
    this->execute(nullptr);
}

void Worker::reset() { m_currentstep = WS_INIT; }

void Worker::init_step() {
    m_status->filepath = state::context->file->source.c_str();
    m_status->filesize = state::context->file->size();
    m_status->loader = state::context->loader->name;
    m_status->processor = state::context->processor->name;
    m_status->analysisstart = std::time(nullptr);

    state::context->process_listing(); // Show pre-analysis listing
    m_status->listingchanged = true;
    m_currentstep++;
}

void Worker::emulate_step() {
    if(emulator.has_pending_code()) {
        emulator.tick();
        auto a = state::context->index_to_address(emulator.current);
        a.map([&](RDAddress address) { m_status->address.value = address; });
        m_status->address.valid = a.has_value();
    }
    else
        m_currentstep++;
}

void Worker::analyze_step() {
    state::context->process_segments(false); // Show pre-analysis listing
    m_status->listingchanged = true;

    for(const RDAnalyzer& a : state::context->analyzers) {
        if((a.flags & ANA_RUNONCE) && (m_analyzerruns[a.name] > 0))
            continue;

        m_analyzerruns[a.name]++;

        if(a.execute && state::context->selectedanalyzers.contains(a.name))
            a.execute(&a);
    }

    if(this->emulator.has_pending_code()) {
        if(m_currentstep == STEP_ANALYZE1)
            m_currentstep = WS_EMULATE1;
        else if(m_currentstep == STEP_ANALYZE2)
            m_currentstep = STEP_EMULATE2;
        else
            unreachable;
    }
    else
        m_currentstep++;
}

void Worker::mergecode_step() {
    const Context* ctx = state::context;
    const auto& mem = ctx->memory;

    for(const Segment& seg : ctx->segments) {
        if(!(seg.type & SEG_HASCODE) || seg.offset == seg.endoffset)
            continue;

        usize idx = seg.index;

        while(idx < seg.endindex && idx < mem->size()) {
            Byte b = mem->at(idx);

            if(b.has_byte() && b.is_unknown()) {
                this->emulator.enqueue_flow(idx++);

                // Move after the unknown range
                while(idx < seg.endindex && idx < mem->size() &&
                      mem->at(idx).is_unknown())
                    idx++;
            }
            else
                idx++;
        }
    }

    if(this->emulator.has_pending_code())
        m_currentstep = STEP_EMULATE2;
    else
        m_currentstep++;
}

void Worker::mergedata_step() {
    Context* ctx = state::context;
    auto& mem = ctx->memory;

    for(usize idx = 0; idx < mem->size();) {
        Byte lastb = mem->at(idx);

        if(lastb.is_unknown() && !lastb.has(BF_REFSTO | BF_REFSFROM)) {
            usize startidx = idx++;

            while(idx < mem->size()) {
                Byte b = mem->at(idx);

                // Don't merge inter-segment bytes
                if((idx > startidx && b.has(BF_SEGMENT)) || !b.is_unknown() ||
                   b.has_common() || (b.has_byte() != lastb.has_byte()) ||
                   (b.has_byte() && (b.byte() != lastb.byte())))
                    break;

                idx++;
            }

            usize len = idx - startidx;

            if(len > static_cast<usize>(ctx->processor->integer_size)) {
                mem->set_n(startidx, len, BF_DATA);
                mem->set(startidx, BF_FILL);
            }
        }
        else
            idx++;
    }

    m_currentstep++;
}

void Worker::finalize_step() {
    state::context->process_segments(true);
    state::context->process_listing();
    m_status->listingchanged = true;
    m_currentstep++;
}

} // namespace redasm
