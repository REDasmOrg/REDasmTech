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
    m_status->stepscurrent = m_currentstep;
    m_status->address.valid = false;

    if(busy) {
        switch(m_currentstep) {
            case STEP_INIT: this->init_step(); break;
            case STEP_EMULATE: this->emulate_step(); break;
            case STEP_CFG: this->cfg_step(); break;
            case STEP_ANALYZE: this->analyze_step(); break;
            default: unreachable;
        }
    }
    else
        state::context->build_listing();

    if(s)
        *s = m_status.get();

    return busy;
}

void Disassembler::execute(usize step) {
    if(step == m_currentstep)
        return;

    this->set_step(step);
    this->execute(nullptr);
}

void Disassembler::set_step(usize s) { m_currentstep = s; }

void Disassembler::next_step() {
    m_currentstep = std::min<usize>(m_currentstep + 1, STEP_DONE);
}

void Disassembler::init_step() {
    static constexpr std::array<const char*, STEP_LAST> STEPS_LIST = {
        "Init", "Emulate", "CFG", "Analyze", "Done",
    };

    m_status->filepath = state::context->file->source.c_str();
    m_status->filesize = state::context->file->size();

    m_status->loader = state::context->loader->name;
    m_status->processor = state::context->processor->name;

    m_status->analysisstart = std::time(nullptr);

    m_status->stepslist = STEPS_LIST.data();
    m_status->stepscount = STEPS_LIST.size();

    state::context->build_listing();
    this->next_step();
}

void Disassembler::emulate_step() {
    if(emulator.has_next()) {

        auto a = emulator.get_next_address();
        if(a)
            m_status->address.value = *a;
        m_status->address.valid = a.has_value();

        emulator.next();
    }
    else
        this->next_step();
}

void Disassembler::analyze_step() {
    for(const RDAnalyzer& a : state::analyzers) {
        if(a.execute && state::context->selectedanalyzers.count(&a))
            a.execute(&a);
    }

    this->next_step();
}

void Disassembler::cfg_step() { this->next_step(); }

} // namespace redasm
