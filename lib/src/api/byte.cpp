#include "marshal.h"
#include <redasm/byte.h>

bool rdbyte_isunknown(const RDByte* self) {
    spdlog::trace("rdbyte_isunknown({})", fmt::ptr(self));
    return self && ((*self & BF_MUNKN) == BF_UNKNOWN);
}

bool rdbyte_iscode(RDByte self) {
    spdlog::trace("rdbyte_iscode({:#04x})", self);
    return redasm::api::from_c(self).is_code();
}

bool rdbyte_isdata(RDByte self) {
    spdlog::trace("rdbyte_isdata({:#04x})", self);
    return redasm::api::from_c(self).is_data();
}

bool rdbyte_hascommon(const RDByte* self) {
    return rdbyte_hasflag(self, BF_MCOMM);
}

bool rdbyte_hasflag(const RDByte* self, usize f) {
    spdlog::trace("rdbyte_hasflag({}, {:#04x})", fmt::ptr(self), f);
    return self && ((*self & f) == f);
}

bool rdbyte_hasbyte(const RDByte* self) {
    spdlog::trace("rdbyte_hasbyte({})", fmt::ptr(self));
    return rdbyte_hasflag(self, BF_BYTE);
}

bool rdbyte_getbyte(const RDByte* self, u8* b) {
    spdlog::trace("rdbyte_hasbyte({}, {})", fmt::ptr(self), fmt::ptr(b));

    if(rdbyte_hasbyte(self)) {
        if(b) *b = static_cast<u8>(*self & 0xFF);
        return true;
    }

    return false;
}

void rdbyte_setflag(RDByte* self, u32 f, bool s) {
    if(!self) return;

    if(s)
        *self |= f;
    else
        *self &= ~f;
}

void rdbyte_setbyte(RDByte* self, u8 b) {
    if(self) {
        *self &= ~BF_MBYTE;
        rdbyte_setflag(self, (b | BF_BYTE), true);
    }
}

void rdbyte_clear(RDByte* self) {
    if(self) *self &= BF_MMASK;
}
