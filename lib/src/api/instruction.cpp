#include <algorithm>
#include <redasm/instruction.h>
#include <spdlog/spdlog.h>
#include <string>

void rdinstruction_setmnemonic(RDInstruction* instr, const char* mnemonic) {
    spdlog::trace("rdinstruction_setmnemonic({}, '{}')", fmt::ptr(instr),
                  mnemonic);
    if(!instr || !mnemonic) return;

    if(*mnemonic == '\0') {
        *instr->mnemonic = '\0';
        return;
    }

    size_t len = std::char_traits<char>::length(mnemonic);
    if(len >= RD_NMNEMONIC) len = RD_NMNEMONIC - 1;

    std::copy_n(mnemonic, len, instr->mnemonic);
    instr->mnemonic[len] = '\0';
}

usize rdinstruction_operandscount(const RDInstruction* instr) {
    spdlog::trace("rdinstruction_operandscount({})", fmt::ptr(instr));
    int c = 0;

    while(instr && instr->operands[c].type != OP_NULL)
        c++;
    return c;
}
