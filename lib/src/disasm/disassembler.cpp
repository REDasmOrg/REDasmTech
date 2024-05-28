#include "disassembler.h"
#include "../context.h"
#include "../state.h"
#include <algorithm>

namespace redasm {

Disassembler::Disassembler() {}

void Disassembler::execute() {
    while(this->status.stepscurrent < ENGINESTATE_DONE) {
        switch(this->status.stepscurrent) {
            case ENGINESTATE_STOP:
                this->status.analysisstart = static_cast<u64>(time(nullptr));
                this->next_step();
                break;

            case ENGINESTATE_EMULATE: this->emulate_step(); break;
            case ENGINESTATE_CFG: this->cfg_step(); break;
            case ENGINESTATE_ANALYZE: this->analyze_step(); break;
            default: unreachable;
        }
    }
}

void Disassembler::execute(usize step) {
    if(step == this->status.stepscurrent)
        return;

    this->set_step(step);
    this->execute();
}

void Disassembler::set_step(usize s) { this->status.stepscurrent = s; }

void Disassembler::next_step() {
    this->status.stepscurrent =
        std::min<usize>(this->status.stepscurrent + 1, ENGINESTATE_DONE);
}

void Disassembler::emulate_step() {
    while(emulator.has_next())
        emulator.next();

    this->next_step();
}

void Disassembler::analyze_step() { this->next_step(); }

void Disassembler::cfg_step() { this->next_step(); }

} // namespace redasm
