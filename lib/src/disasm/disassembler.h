#pragma once

#include "emulator.h"
#include <memory>
#include <redasm/engine.h>

namespace redasm {

class Disassembler {
public:
    Disassembler();
    bool execute(const RDAnalysisStatus** s);
    void execute(usize step);
    void reset() { m_currentstep = STEP_INIT; }

private:
    void next_step();
    void init_step();
    void emulate_step();
    void analyze_step();
    void process_step();

public:
    Emulator emulator;

private:
    std::unordered_map<std::string_view, usize> m_analyzerruns;
    std::unique_ptr<RDAnalysisStatus> m_status;
    usize m_currentstep{STEP_INIT};
};

} // namespace redasm
