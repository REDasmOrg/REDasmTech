#include "../memory/mbyte.h"
#include <redasm/byte.h>
#include <spdlog/spdlog.h>

bool rdmbyte_isunknown(RDMByte self) {
    spdlog::trace("rdmbyte_isunknown({:#04x})", self);
    return redasm::mbyte::is_unknown(self);
}

bool rdmbyte_iscode(RDMByte self) {
    spdlog::trace("rdmbyte_iscode({:#04x})", self);
    return redasm::mbyte::is_code(self);
}

bool rdmbyte_isdata(RDMByte self) {
    spdlog::trace("rdmbyte_isdata({:#04x})", self);
    return redasm::mbyte::is_data(self);
}

bool rdmbyte_hascommon(RDMByte self) {
    spdlog::trace("rdmbyte_hascommon({:#04x})", self);
    return redasm::mbyte::has_common(self);
}

bool rdmbyte_hasflag(RDMByte self, usize f) {
    spdlog::trace("rdmbyte_hasflag({}, {:#04x})", self, f);
    return redasm::mbyte::has_flag(self, f);
}

bool rdmbyte_hasbyte(RDMByte self) {
    spdlog::trace("rdmbyte_hasbyte({:#04x})", self);
    return redasm::mbyte::has_byte(self);
}

bool rdmbyte_getbyte(RDMByte self, u8* b) {
    spdlog::trace("rdmbyte_hasbyte({:#04x}, {})", self, fmt::ptr(b));

    if(redasm::mbyte::has_byte(self)) {
        *b = redasm::mbyte::get_byte(self);
        return true;
    }

    return false;
}

void rdmbyte_setflag(RDMByte* self, u32 f, bool s) {
    spdlog::trace("rdmbyte_hasbyte({}, {:#04x}, {})", fmt::ptr(self), f, s);
    redasm::mbyte::set_flag(self, f, s);
}

void rdmbyte_set(RDMByte* self, u32 f) {
    spdlog::trace("rdmbyte_set({}, {:#04x})", fmt::ptr(self), f);
    redasm::mbyte::set(self, f);
}

void rdmbyte_unset(RDMByte* self, u32 f) {
    spdlog::trace("rdmbyte_unset({}, {:#04x})", fmt::ptr(self), f);
    redasm::mbyte::unset(self, f);
}

void rdmbyte_setbyte(RDMByte* self, u8 b) {
    spdlog::trace("rdmbyte_setbyte({}, {})", fmt::ptr(self), b);
    redasm::mbyte::set_byte(self, b);
}

void rdmbyte_clear(RDMByte* self) {
    spdlog::trace("rdmbyte_clear({})", fmt::ptr(self));
    redasm::mbyte::clear(self);
}
