#include "arm/arm32.h"
#include <redasm/redasm.h>

namespace {

template<typename Processor>
void compile_fields(RDProcessorPlugin* plugin, const char* id, const char* name,
                    int addrsize, int intsize) {
    plugin->level = REDASM_API_LEVEL;
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

    plugin->normalize_address = [](const RDProcessor* self, RDAddress addr) {
        return reinterpret_cast<const Processor*>(self)->normalize_address(
            addr);
    };

    plugin->get_mnemonic = [](const RDProcessor* self,
                              const RDInstruction* instr) {
        return reinterpret_cast<const Processor*>(self)->get_mnemonic(instr);
    };

    plugin->get_segmentregisters = [](const RDProcessor* self) {
        return reinterpret_cast<const Processor*>(self)->get_segmentregisters();
    };

    plugin->get_registername = [](const RDProcessor* self, int regid) {
        return reinterpret_cast<const Processor*>(self)->get_registername(
            regid);
    };

    plugin->setup = [](RDProcessor* self, RDEmulator* e) {
        reinterpret_cast<Processor*>(self)->setup(e);
    };

    plugin->decode = [](RDProcessor* self, RDInstruction* instr) {
        reinterpret_cast<Processor*>(self)->decode(instr);
    };

    plugin->emulate = [](RDProcessor* self, RDEmulator* e,
                         const RDInstruction* instr) {
        reinterpret_cast<const Processor*>(self)->emulate(e, instr);
    };

    plugin->render_instruction = [](const RDProcessor* self, RDRenderer* r,
                                    const RDInstruction* instr) {
        reinterpret_cast<const Processor*>(self)->render_instruction(r, instr);
    };
}

RDProcessorPlugin arm32le;
RDProcessorPlugin arm32be;
RDProcessorPlugin thumble;
RDProcessorPlugin thumbbe;

} // namespace

void rdplugin_create() {
    // clang-format off
    compile_fields<arm32::ARMLE>(&arm32le, "arm32le", "ARM (Little Endian)", 4, 4);
    compile_fields<arm32::ARMBE>(&arm32be, "arm32be", "ARM (Big Endian)", 4, 4);
    compile_fields<arm32::THUMBLE>(&thumble, "thumble", "THUMB (Little Endian)", 2, 4);
    compile_fields<arm32::THUMBBE>(&thumbbe, "thumbbe", "THUMB (Big Endian)", 2, 4);
    // clang-format on

    rd_registerprocessor(&arm32le);
    rd_registerprocessor(&arm32be);
    rd_registerprocessor(&thumble);
    rd_registerprocessor(&thumbbe);
}
