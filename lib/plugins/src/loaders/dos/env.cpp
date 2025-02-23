#include "env.h"

namespace env {

namespace {

RDEnvironment msdos{};

void update_instruction(RDInstruction* instr) {
    if(rd_matchmnemonic(instr, "int")) {
        switch(instr->operands[0].imm) {
            case 0x27: // Terminate and stay resident
                instr->features |= IF_STOP;
                break;
            default: break;
        }
    }
}

} // namespace

const RDEnvironment* get_environment(RDLoader*) {
    if(!msdos.name) rdenvironment_init(&env::msdos, "MS-DOS");
    msdos.update_instruction = env::update_instruction;
    return &msdos;
}

} // namespace env
