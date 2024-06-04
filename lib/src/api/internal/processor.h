#pragma once

#include <redasm/processor.h>
#include <string_view>

namespace redasm::api::internal {

void emulator_addcoderef(RDEmulator* self, RDAddress address, usize f);
void emulator_adddataref(RDEmulator* self, RDAddress address, usize f);
void emulator_settype(RDEmulator* self, RDAddress address,
                      std::string_view tname);

usize get_processors(const RDProcessor** processors);
void register_processor(const RDProcessor& processor);
void set_processor(std::string_view name);

} // namespace redasm::api::internal
