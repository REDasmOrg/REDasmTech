#pragma once

#include <redasm/buffer.h>
#include <redasm/types.h>
#include <redasm/typing.h>
#include <tl/optional.hpp>

namespace redasm::utils {

tl::optional<RDLEB128> decode_uleb128(const RDBuffer* buffer, usize idx);
tl::optional<RDLEB128> decode_leb128(const RDBuffer* buffer, usize idx);

} // namespace redasm::utils
