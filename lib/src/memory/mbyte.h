#pragma once

#include "../error.h"
#include <redasm/byte.h>
#include <redasm/types.h>

namespace redasm::mbyte {

inline bool has_flag(RDMByte self, u32 f) { return (self & f) == f; }
inline bool is_unknown(RDMByte self) { return (self & BF_MUNKN) == BF_UNKNOWN; }
inline bool is_code(RDMByte self) { return (self & BF_CODE) == BF_CODE; }
inline bool is_data(RDMByte self) { return (self & BF_DATA) == BF_DATA; }
inline bool is_weak(RDMByte self) { return mbyte::has_flag(self, BF_WEAK); }
inline bool is_start(RDMByte self) { return mbyte::has_flag(self, BF_START); }
inline bool is_end(RDMByte self) { return mbyte::has_flag(self, BF_END); }
inline bool is_cont(RDMByte self) { return mbyte::has_flag(self, BF_CONT); }
inline bool has_byte(RDMByte self) { return mbyte::has_flag(self, BF_BYTE); }
inline bool has_common(RDMByte self) { return mbyte::has_flag(self, BF_MCOMM); }

inline u8 get_byte(RDMByte self) {
    assume(mbyte::has_byte(self));
    return static_cast<u8>(self & 0xFF);
}

inline void set_flag(RDMByte* self, u32 f, bool b) {
    if(!self) return;

    if(b)
        *self |= f;
    else
        *self &= ~f;
}

inline void set(RDMByte* self, u32 f) { mbyte::set_flag(self, f, true); }
inline void unset(RDMByte* self, u32 f) { mbyte::set_flag(self, f, false); }

inline void clear(RDMByte* self) {
    if(self) *self &= BF_MMASK;
}

inline void set_byte(RDMByte* self, u8 byte) {
    if(!self) return;
    *self &= ~BF_MBYTE;
    mbyte::set(self, byte | BF_BYTE);
}

static_assert(sizeof(RDMByte) == sizeof(u32));

} // namespace redasm::mbyte
