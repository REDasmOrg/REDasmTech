#include "marshal.h"
#include <redasm/byte.h>

bool rdbyte_isunknown(RDByte self) {
    spdlog::trace("rdbyte_isunknown({:#04x})", self);
    return redasm::api::from_c(self).is_unknown();
}
bool rdbyte_iscode(RDByte self) {
    spdlog::trace("rdbyte_iscode({:#04x})", self);
    return redasm::api::from_c(self).is_code();
}

bool rdbyte_isdata(RDByte self) {
    spdlog::trace("rdbyte_isdata({:#04x})", self);
    return redasm::api::from_c(self).is_data();
}

bool rdbyte_has(RDByte self, usize f) {
    spdlog::trace("rdbyte_isdata({:#04x})", self);
    return redasm::api::from_c(self).has(f);
}

bool rdbyte_hasbyte(RDByte self) {
    spdlog::trace("rdbyte_hasbyte({:#04x})", self);
    return redasm::api::from_c(self).has_byte();
}

bool rdbyte_getbyte(RDByte self, u8* b) {
    spdlog::trace("rdbyte_hasbyte({:#04x}, {})", self, fmt::ptr(b));
    redasm::Byte mb = redasm::api::from_c(self);

    if(mb.has_byte()) {
        if(b) *b = mb.byte();
        return true;
    }

    return false;
}
