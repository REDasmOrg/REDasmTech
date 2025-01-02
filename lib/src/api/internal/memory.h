#pragma once

#include "../../memory/byte.h"
#include <redasm/memory.h>
#include <redasm/segment.h>
#include <tl/optional.hpp>

namespace redasm::api::internal {

bool memory_map(RDAddress startaddr, RDAddress endaddr);
bool memory_map_n(RDAddress base, usize size);
void memory_copy(RDAddress address, RDOffset start, RDOffset end);
void memory_copy_n(RDAddress address, RDOffset start, usize size);
usize memory_bytes(const Byte** b);
void memory_info(RDMemoryInfo* mi);
usize memory_size();
bool is_address(RDAddress address);

bool check_string(RDAddress address, RDStringResult* mi);
usize read(RDAddress address, void* dst, usize n);
tl::optional<bool> get_bool(RDAddress address);
tl::optional<char> get_char(RDAddress address);
tl::optional<u8> get_u8(RDAddress address);
tl::optional<u16> get_u16(RDAddress address);
tl::optional<u32> get_u32(RDAddress address);
tl::optional<u64> get_u64(RDAddress address);
tl::optional<i8> get_i8(RDAddress address);
tl::optional<i16> get_i16(RDAddress address);
tl::optional<i32> get_i32(RDAddress address);
tl::optional<i64> get_i64(RDAddress address);
tl::optional<u16> get_u16be(RDAddress address);
tl::optional<u32> get_u32be(RDAddress address);
tl::optional<u64> get_u64be(RDAddress address);
tl::optional<i16> get_i16be(RDAddress address);
tl::optional<i32> get_i32be(RDAddress address);
tl::optional<i64> get_i64be(RDAddress address);
tl::optional<std::string> get_strz(RDAddress address);
tl::optional<std::string> get_str(RDAddress address, usize n);

} // namespace redasm::api::internal
