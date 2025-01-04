#pragma once

// http://pallergabor.uw.hu/androidblog/dalvik_opcodes.html

#include <redasm/redasm.h>

namespace dalvik {

const char* get_registername(const RDProcessor*, int regid);
void decode(const RDProcessor*, RDInstruction* instr);
void emulate(const RDProcessor*, RDEmulator* e, const RDInstruction* instr);
void render_instruction(const RDProcessor*, RDRenderer* r,
                        const RDInstruction* instr);

} // namespace dalvik
