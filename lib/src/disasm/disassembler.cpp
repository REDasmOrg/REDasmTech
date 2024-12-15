#include "disassembler.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"
#include <algorithm>
#include <ctime>

namespace redasm {

Disassembler::Disassembler() { m_status = std::make_unique<RDEngineStatus>(); }

bool Disassembler::execute(const RDAnalysisStatus** s) {
    bool busy = m_currentstep < STEP_DONE;
    m_status->step = m_currentstep;
    m_status->address.valid = false;

    if(busy) {
        switch(m_currentstep) {
            case STEP_INIT: this->init_step(); break;
            case STEP_EMULATE: this->emulate_step(); break;
            case STEP_PROCESS: this->process_step(); break;
            case STEP_ANALYZE: this->analyze_step(); break;
            case STEP_TYPES: this->types_step(); break;
            default: unreachable;
        }
    }
    else
        state::context->process_listing();

    if(s)
        *s = m_status.get();

    return busy;
}

void Disassembler::execute(usize step) {
    if(step == m_currentstep)
        return;

    m_currentstep = step;
    this->execute(nullptr);
}

void Disassembler::next_step() {
    if(emulator.has_pending_code())
        m_currentstep = STEP_EMULATE; // Repeat emulation
    else
        m_currentstep = std::min<usize>(m_currentstep + 1, STEP_DONE);
}

void Disassembler::init_step() {
    static constexpr std::array<const char*, STEP_COUNT> STEPS_LIST = {
        "Init", "Emulate", "Process", "Analyze", "Types", "Done",
    };

    m_status->filepath = state::context->file->source.c_str();
    m_status->filesize = state::context->file->size();

    m_status->loader = state::context->loader->name;
    m_status->processor = state::context->processor->name;

    m_status->analysisstart = std::time(nullptr);

    m_status->stepslist = STEPS_LIST.data();
    m_status->stepscount = STEPS_LIST.size();

    state::context->process_listing();
    this->next_step();
}

void Disassembler::emulate_step() {
    if(emulator.has_pending_code()) {
        emulator.tick();
        auto a = state::context->index_to_address(emulator.current);
        a.map([&](RDAddress address) { m_status->address.value = address; });
        m_status->address.valid = a.has_value();
    }
    else
        m_currentstep = STEP_PROCESS;
}

void Disassembler::analyze_step() {
    for(const RDAnalyzer& a : state::context->analyzers) {
        if((a.flags & ANA_RUNONCE) && (m_analyzerruns[a.name] > 0))
            continue;

        m_analyzerruns[a.name]++;

        if(a.execute && state::context->selectedanalyzers.contains(a.name))
            a.execute(&a);
    }

    this->next_step();
}

void Disassembler::types_step() {
    spdlog::info("Propagating types...");

    if(emulator.has_pending_types()) {
        emulator.tick_type();
        auto a = state::context->index_to_address(emulator.current);
        a.map([&](RDAddress address) { m_status->address.value = address; });
        m_status->address.valid = a.has_value();
    }
    else
        this->next_step();
}

void Disassembler::process_step() {
    state::context->process_segments();
    this->next_step();
}

} // namespace redasm
