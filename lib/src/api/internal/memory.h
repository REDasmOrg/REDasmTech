#pragma once

#include "../../memory/byte.h"
#include "../../typing/typing.h"
#include <redasm/memory.h>
#include <tl/optional.hpp>

namespace redasm::api::internal {

bool check_string(RDAddress address, RDStringResult* mi);
bool memory_map(RDAddress startaddr, RDAddress endaddr);
bool memory_map_n(RDAddress base, usize size);
void memory_copy(RDAddress address, RDOffset start, RDOffset end);
void memory_copy_n(RDAddress address, RDOffset start, usize size);
tl::optional<typing::Value> map_type(RDAddress address, std::string_view tname);
usize memory_bytes(const Byte** b);
void memory_info(RDMemoryInfo* mi);
usize memory_read(RDAddress address, char* data, usize n);
usize memory_size();

} // namespace redasm::api::internal
