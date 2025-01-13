#include "arm/arm32.h"
#include <redasm/redasm.h>

namespace {

template<typename Processor>
void compile_fields(RDProcessorPlugin* plugin, const char* id, const char* name,
                    int addrsize, int intsize) {
    plugin->id = id;
    plugin->name = name;
    plugin->address_size = addrsize;
    plugin->integer_size = intsize;

    plugin->create = [](const RDProcessorPlugin*) {
        return reinterpret_cast<RDProcessor*>(new Processor());
    };

    plugin->destroy = [](RDProcessor* self) {
        delete reinterpret_cast<Processor*>(self);
    };

    plugin->getmnemonic = [](const RDProcessor* self, u32 id) {
        return reinterpret_cast<const Processor*>(self)->get_mnemonic(id);
    };

    plugin->getregistername = [](const RDProcessor* self, int regid) {
        return reinterpret_cast<const Processor*>(self)->get_registername(
            regid);
    };

    plugin->decode = [](RDProcessor* self, RDInstruction* instr) {
        reinterpret_cast<Processor*>(self)->decode(instr);
    };

    plugin->emulate = [](RDProcessor* self, RDEmulator* e,
                         const RDInstruction* instr) {
        reinterpret_cast<const Processor*>(self)->emulate(e, instr);
    };

    plugin->renderinstruction = [](const RDProcessor* self, RDRenderer* r,
                                   const RDInstruction* instr) {
        reinterpret_cast<const Processor*>(self)->render_instruction(r, instr);
    };
}

RDProcessorPlugin arm32le;
RDProcessorPlugin arm32be;

} // namespace

void rdplugin_create() {
    // clang-format off
    compile_fields<ARM32LEProcessor>(&arm32le, "arm32le", "ARM (Little Endian)", 4, 4);
    compile_fields<ARM32BEProcessor>(&arm32be, "arm32be", "ARM (Big Endian)", 4, 4);
    // clang-format on

    rd_registerprocessor(&arm32le);
    rd_registerprocessor(&arm32be);
}
