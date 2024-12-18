#pragma once

#include <redasm/processor.h>
#include <string_view>

namespace redasm::api::internal {

void emulator_addref(RDEmulator* e, RDAddress toaddr, usize type);
bool emulator_settype(RDEmulator* e, RDAddress address, const RDType* type);

usize get_processors(const RDProcessor** processors);
void register_processor(const RDProcessor& processor);
void set_processor(std::string_view id);

} // namespace redasm::api::internal
