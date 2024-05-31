#pragma once

#include <redasm/processor.h>
#include <string_view>

namespace redasm::api::internal {

usize get_processors(const RDProcessor** processors);
void register_processor(const RDProcessor& processor);
void set_processor(std::string_view name);

} // namespace redasm::api::internal
