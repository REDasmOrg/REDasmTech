#include "mips_decoder.h"

namespace {

RDProcessor mips32le;
RDProcessor mips32be;
// RDProcessor mips64le;
// RDProcessor mips64be;

template<bool BigEndian>
void decode(const RDProcessor*, RDInstruction* instr) {
    MIPSDecodedInstruction dec;
    if(!mips_decoder::decode(instr->address, dec, BigEndian))
        return;

    instr->id = dec.opcode->id;
    instr->length = dec.length;
}

void emulate(const RDProcessor*, RDEmulator* e, const RDInstruction* instr) {}

void render_instruction(const RDProcessor*, const RDRendererParams* rp,
                        const RDInstruction* instr) {}

} // namespace

void rdplugin_init() {
    mips_initialize_formats();

    mips32le.id = "mips32le";
    mips32le.name = "MIPS32 (Little Endian)";
    mips32le.decode = decode<false>;
    mips32le.emulate = emulate;
    mips32le.renderinstruction = render_instruction;
    rd_registerprocessor(&mips32le);

    mips32be.id = "mips32be";
    mips32be.name = "MIPS32 (Big Endian)";
    mips32be.decode = decode<true>;
    mips32be.emulate = emulate;
    mips32be.renderinstruction = render_instruction;
    rd_registerprocessor(&mips32be);

    // ...
    // rd_registerprocessor(&mips64le);

    // ...
    // rd_registerprocessor(&mips64be);
}
