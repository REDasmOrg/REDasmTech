#pragma once

#include "../../plugins/origin.h"
#include <redasm/processor.h>
#include <string>
#include <string_view>

namespace redasm::api::internal {

u32 emulator_getdslotinfo(const RDEmulator* self, const RDInstruction** dslot);
void emulator_addref(RDEmulator* self, RDAddress toaddr, usize type);
void emulator_flow(RDEmulator* self, RDAddress flowaddr);
u64 emulator_getreg(const RDEmulator* self, int regid);
void emulator_setreg(RDEmulator* self, int regid, u64 val);
u64 emulator_updreg(RDEmulator* self, int regid, u64 val, u64 mask);
u64 emulator_getstate(const RDEmulator* self, std::string_view state);
u64 emulator_takestate(RDEmulator* self, std::string_view state);
void emulator_delstate(RDEmulator* self, std::string_view state);
void emulator_setstate(RDEmulator* self, const std::string& state, u64 val);
u64 emulator_updstate(RDEmulator* self, std::string_view state, u64 val,
                      u64 mask);

bool register_processor(const RDProcessorPlugin* plugin, pm::Origin o);
const RDProcessorPlugin** get_processorplugins(usize* n);
const RDProcessorPlugin* get_processorplugin();
const RDProcessor* get_processor();
bool decode(RDAddress address, RDInstruction* instr);

} // namespace redasm::api::internal
