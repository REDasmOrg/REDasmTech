#pragma once

#include "emulator.h"
#include <memory>
#include <redasm/worker.h>

namespace redasm {

class Worker {

public:
    Worker();
    bool execute(const RDWorkerStatus** s);
    void execute(usize step);

private:
    void init_step();
    void emulate_step();
    void analyze_step();
    void mergecode_step();
    void mergedata_step();
    void signature_step();
    void finalize_step();

public:
    Emulator emulator;

private:
    std::unordered_map<std::string_view, usize> m_analyzerruns;
    std::unique_ptr<RDWorkerStatus> m_status;
    usize m_currentstep;
};

} // namespace redasm
