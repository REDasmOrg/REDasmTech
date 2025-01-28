#include "../utils/hash.h"
#include <redasm/hash.h>
#include <spdlog/spdlog.h>

u32 rd_adler32(const void* data, usize size) {
    spdlog::trace("rd_adler32({}, {})", fmt::ptr(data), size);
    return redasm::hash::adler32(data, size);
}
