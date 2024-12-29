#include "../internal/memory.h"
#include <redasm/memory.h>

usize rd_memoryread(RDAddress address, char* data, usize n) {
    return redasm::api::internal::memory_read(address, data, n);
}

void rd_memoryinfo(RDMemoryInfo* mi) { redasm::api::internal::memory_info(mi); }

bool rd_map(RDAddress startaddr, RDAddress endaddr) {
    return redasm::api::internal::memory_map(startaddr, endaddr);
}

bool rd_map_n(RDAddress baseaddress, usize size) {
    return redasm::api::internal::memory_map_n(baseaddress, size);
}
