#include "worker.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"
#include "memprocess.h"
#include <ctime>

namespace redasm {

namespace {

// clang-format off
enum WorkerSteps {
    WS_INIT = 0,
    WS_EMULATE1, WS_ANALYZE1, // First pass
    WS_MERGECODE,
    WS_EMULATE2, WS_ANALYZE2, // Second pass
    WS_MERGEDATA,
    WS_FINALIZE,
    WS_DONE,
    WS_COUNT,
};
// clang-format on

constexpr std::array<const char*, WS_COUNT> WS_NAMES = {
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
    m_status->busy = m_currentstep < WS_DONE;
    m_status->currentstep = WS_NAMES.at(m_currentstep);
    m_status->address.valid = false;
    m_status->listingchanged = false;

    if(m_status->busy) {
        switch(m_currentstep) {
            case WS_INIT: this->init_step(); break;

            case WS_EMULATE1:
            case WS_EMULATE2: this->emulate_step(); break;

            case WS_ANALYZE1:
            case WS_ANALYZE2: this->analyze_step(); break;

            case WS_MERGECODE: this->mergecode_step(); break;
            case WS_MERGEDATA: this->mergedata_step(); break;
            case WS_FINALIZE: this->finalize_step(); break;
            default: unreachable;
        }
    }
    else {
        mem::process_listing();
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

    mem::process_listing(); // Show pre-analysis listing
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
    mem::process_segments(false); // Show pre-analysis listing
    m_status->listingchanged = true;

    for(const RDAnalyzer& a : state::context->analyzers) {
        if((a.flags & ANA_RUNONCE) && (m_analyzerruns[a.name] > 0))
            continue;

        m_analyzerruns[a.name]++;

        if(a.execute && state::context->selectedanalyzers.contains(a.name))
            a.execute(&a);
    }

    if(this->emulator.has_pending_code()) {
        if(m_currentstep == WS_ANALYZE1)
            m_currentstep = WS_EMULATE1;
        else if(m_currentstep == WS_ANALYZE2)
            m_currentstep = WS_EMULATE2;
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
        m_currentstep = WS_EMULATE2;
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
    mem::process_segments(true);
    mem::process_listing();
    m_status->listingchanged = true;
    m_currentstep++;
}

} // namespace redasm
