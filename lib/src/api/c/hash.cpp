#include "../internal/hash.h"
#include <redasm/hash.h>

u32 rd_adler32(const void* data, usize size) {
    return redasm::api::internal::adler32(data, size);
}
