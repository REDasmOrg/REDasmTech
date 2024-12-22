#pragma once

#include <redasm/processor.h>
#include <string>
#include <string_view>

namespace redasm::api::internal {

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

const RDProcessor* get_processor();
usize get_processors(const RDProcessor** processors);
void register_processor(const RDProcessor& processor);
void set_processor(std::string_view id);
bool decode(RDAddress address, RDInstruction* instr);

} // namespace redasm::api::internal
