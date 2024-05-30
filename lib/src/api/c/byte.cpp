#include "../internal/byte.h"
#include <redasm/byte.h>

bool rdbyte_isunknown(RDByte self) {
    return redasm::api::internal::byte_isunknown(self);
}
bool rdbyte_iscode(RDByte self) {
    return redasm::api::internal::byte_iscode(self);
}

bool rdbyte_isdata(RDByte self) {
    return redasm::api::internal::byte_isdata(self);
}

bool rdbyte_has(RDByte self, usize f) {
    return redasm::api::internal::byte_has(self, f);
}

bool rdbyte_hasbyte(RDByte self) {
    return redasm::api::internal::byte_hasbyte(self);
}

bool rdbyte_getbyte(RDByte self, u8* b) {
    return redasm::api::internal::byte_getbyte(self, b);
}
