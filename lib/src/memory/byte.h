#pragma once

#include "../error.h"
#include <redasm/byte.h>
#include <redasm/types.h>

namespace redasm {

struct Byte {
    u32 value;

    [[nodiscard]] inline bool is_unknown() const {
        return (value & BF_UDCMASK) == BF_UNKNOWN;
    }

    [[nodiscard]] inline bool is_code() const {
        return (value & BF_UDCMASK) == BF_CODE;
    }

    [[nodiscard]] inline bool is_data() const {
        return (value & BF_UDCMASK) == BF_DATA;
    }

    [[nodiscard]] inline bool is_cont() const { return this->has(BF_CONT); }
    [[nodiscard]] inline bool is_weak() const { return this->has(BF_WEAK); }
    [[nodiscard]] inline bool has_byte() const { return this->has(BF_HASBYTE); }
    [[nodiscard]] inline bool has(u32 f) const { return value & f; }

    [[nodiscard]] inline u8 byte() const {
        assume(this->has_byte());
        return static_cast<u8>(value & 0xFF);
    }

    inline void set(u32 f) { this->set_flag(f, true); }
    inline void unset(u32 f) { this->set_flag(f, false); }

    inline void set_flag(u32 f, bool b) {
        if(b)
            value |= f;
        else
            value &= ~f;
    }

    inline void set_byte(u8 byte) {
        value &= ~BF_BYTEMASK;
        this->set(byte | BF_HASBYTE);
    }

    inline void mark_data() {
        value &= ~BF_UDCMASK;
        value |= BF_DATA;
    }

    inline void mark_code() {
        value &= ~BF_UDCMASK;
        value |= BF_CODE;
    }

    inline void as_unknown() { value &= ~BF_UDCMASK; }
};

static_assert(sizeof(Byte) == sizeof(u32));
static_assert(sizeof(Byte) == sizeof(RDByte));

} // namespace redasm
