#include "../internal/byte.h"
#include <redasm/byte.h>

bool rdbyte_isunknown(RDByte self) {
    return redasm::api::internal::byte_isunknown(redasm::Byte{self});
}
bool rdbyte_iscode(RDByte self) {
    return redasm::api::internal::byte_iscode(redasm::Byte{self});
}

bool rdbyte_isdata(RDByte self) {
    return redasm::api::internal::byte_isdata(redasm::Byte{self});
}

bool rdbyte_has(RDByte self, RDByteFlags f) {
    return redasm::api::internal::byte_has(redasm::Byte{self}, f);
}

bool rdbyte_hasbyte(RDByte self) {
    return redasm::api::internal::byte_hasbyte(redasm::Byte{self});
}

bool rdbyte_getbyte(RDByte self, u8* b) {
    return redasm::api::internal::byte_getbyte(redasm::Byte{self}, b);
}
