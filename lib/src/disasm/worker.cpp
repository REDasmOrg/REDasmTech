#include "worker.h"
#include "../context.h"
#include "../error.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include "memprocess.h"
#include <ctime>

namespace redasm {

namespace {

// clang-format off
enum WorkerSteps {
    WS_INIT = 0,
    // 1st pass
    WS_EMULATE1, WS_ANALYZE1, 
    WS_MERGEDATA1,
    // 2nd pass
    WS_MERGECODE,
    WS_EMULATE2, WS_ANALYZE2, 
    WS_MERGEDATA2,
    // Finalize pass
    WS_FINALIZE,
    WS_DONE,
    WS_COUNT,
};

constexpr std::array<const char*, WS_COUNT> WS_NAMES = {
    "Init",
    "Emulate #1", "Analyze #1", 
    "Merge Data #1",
    "Merge Code",
    "Emulate #2", "Analyze #2", 
    "Merge Data #2",
    "Finalize", 
    "Done",
};

// clang-format on

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

            case WS_MERGEDATA1:
            case WS_MERGEDATA2: this->mergedata_step(); break;

            case WS_FINALIZE: this->finalize_step(); break;
            default: unreachable;
        }
    }
    else {
        mem::process_listing();
        m_status->listingchanged = true;
    }

    if(s) *s = m_status.get();
    return m_status->busy;
}

void Worker::execute(usize step) {
    if(step == m_currentstep) return;

    m_currentstep = step;
    this->execute(nullptr);
}

void Worker::init_step() {
    m_status->filepath = state::context->file->source.c_str();
    m_status->filesize = state::context->file->size();
    m_status->loader = state::context->loaderplugin->name;
    m_status->processor = state::context->processorplugin->name;
    m_status->analysisstart = std::time(nullptr);

    if(state::context->memory) {
        mem::process_listing(); // Show pre-analysis listing
        m_status->listingchanged = true;
        m_currentstep++;
    }
    else
        m_currentstep = WS_DONE;
}

void Worker::emulate_step() {
    if(emulator.has_pending_code()) {
        emulator.tick();
        auto a = state::context->index_to_address(emulator.pc);
        a.map([&](RDAddress address) { m_status->address.value = address; });
        m_status->address.valid = a.has_value();
    }
    else
        m_currentstep++;
}

void Worker::analyze_step() {
    mem::process_segments(false); // Show pre-analysis listing
    m_status->listingchanged = true;

    const Context* ctx = state::context;

    for(const RDAnalyzerPlugin* plugin : ctx->analyzerplugins) {
        if(!(plugin->flags & AF_SELECTED) ||
           (plugin->flags & AF_RUNONCE && m_analyzerruns[plugin->id] > 0))
            continue;

        m_analyzerruns[plugin->id]++;

        if(plugin->execute) {
            RDAnalyzer* a = pm::create_instance(plugin);
            plugin->execute(a);
            pm::destroy_instance(plugin, a);
        }
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

    if(ctx->loaderplugin->flags & LF_NOMERGECODE) {
        m_currentstep = WS_MERGEDATA2;
        return;
    }

    const auto& mem = ctx->memory;

    for(const Segment& seg : ctx->segments) {
        if(!(seg.type & SEG_HASCODE) || seg.offset == seg.endoffset) continue;

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

    if(ctx->loaderplugin->flags & LF_NOMERGEDATA) {
        m_currentstep++;
        return;
    }

    auto& mem = ctx->memory;

    for(usize idx = 0; idx < mem->size();) {
        Byte b = mem->at(idx);

        if(b.is_unknown() && !b.has(BF_REFSTO | BF_REFSFROM)) {
            usize startidx = idx++;
            const Segment* startseg = ctx->index_to_segment(idx);

            while(idx < mem->size()) {
                const Segment* seg = ctx->index_to_segment(idx);
                // Don't merge different/invalid segments
                if(startseg != seg) break;

                Byte b = mem->at(idx);

                if(!b.is_unknown() || b.has_common() ||
                   (b.has_byte() != b.has_byte()) ||
                   (b.has_byte() && (b.byte() != b.byte())))
                    break;

                idx++;
            }

            usize len = idx - startidx;

            if(len > 1 &&
               len > static_cast<usize>(ctx->processorplugin->integer_size)) {
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
