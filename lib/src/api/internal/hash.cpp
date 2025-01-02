#include "hash.h"
#include "../../utils/hash.h"

namespace redasm::api::internal {

u32 adler32(const void* data, usize size) { return hash::adler32(data, size); }

} // namespace redasm::api::internal
