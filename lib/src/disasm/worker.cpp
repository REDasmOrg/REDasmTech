#include "worker.h"
#include "../context.h"
#include "../error.h"
#include "../internal/buffer_internal.h"
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
    m_status->segment = nullptr;
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
        memprocess::process_listing();
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
    m_status->filepath = state::context->program.file->source;
    m_status->filesize = state::context->program.file->length;
    m_status->loader = state::context->loaderplugin->name;
    m_status->processor = state::context->processorplugin->name;

    memprocess::process_listing(); // Show pre-analysis listing
    m_status->listingchanged = true;
    m_currentstep++;
}

void Worker::emulate_step() {
    if(emulator.has_pending_code()) {
        emulator.tick();
        assume(emulator.segment);
        m_status->segment = emulator.segment;
        m_status->address.value = emulator.pc;
        m_status->address.valid = true;
    }
    else
        m_currentstep++;
}

void Worker::analyze_step() {
    memprocess::process_memory(); // Show pre-analysis listing
    m_status->listingchanged = true;

    const Context* ctx = state::context;

    for(const RDAnalyzerPlugin* plugin : ctx->analyzerplugins) {
        if(!ctx->selectedanalyzerplugins.contains(plugin) ||
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
    if(state::context->loaderplugin->flags & LF_NOMERGECODE) {
        m_currentstep = WS_MERGEDATA2;
        return;
    }

    memprocess::merge_code(&this->emulator);

    if(this->emulator.has_pending_code())
        m_currentstep = WS_EMULATE2;
    else
        m_currentstep++;
}

void Worker::mergedata_step() {
    memprocess::process_memory();
    m_currentstep++;
}

void Worker::finalize_step() {
    memprocess::process_listing();
    m_status->listingchanged = true;
    m_currentstep++;
}

} // namespace redasm
