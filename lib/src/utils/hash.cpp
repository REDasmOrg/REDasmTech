#include "hash.h"
#include <miniz.h>

namespace redasm::hash {

u32 adler32(const void* data, usize size) {
    return mz_adler32(MZ_ADLER32_INIT, reinterpret_cast<const u8*>(data), size);
}

} // namespace redasm::hash
