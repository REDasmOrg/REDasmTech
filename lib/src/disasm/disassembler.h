#pragma once

#include "emulator.h"
#include <redasm/engine.h>

namespace redasm {

class Disassembler {
private:
    enum Steps {
        ENGINESTATE_STOP,
        ENGINESTATE_EMULATE,
        ENGINESTATE_CFG,
        ENGINESTATE_ANALYZE,
        ENGINESTATE_DONE,
        ENGINESTATE_LAST
    };

public:
    Disassembler();
    void execute();
    void execute(usize step);

    inline void reset() { this->status.stepscurrent = ENGINESTATE_STOP; }

private:
    void set_step(usize s);
    void next_step();
    void emulate_step();
    void analyze_step();
    void cfg_step();

public:
    RDEngineStatus status{};
    Emulator emulator;
};

} // namespace redasm
