#pragma once

#include "../error.h"
#include <redasm/byte.h>
#include <redasm/types.h>

namespace redasm {

struct Byte {
    u32 value;

    [[nodiscard]] inline bool is_unknown() const {
        return (value & BF_MUNKN) == BF_UNKNOWN;
    }

    [[nodiscard]] inline bool is_code() const {
        return (value & BF_CODE) == BF_CODE;
    }

    [[nodiscard]] inline bool is_data() const {
        return (value & BF_DATA) == BF_DATA;
    }

    [[nodiscard]] inline bool is_cont() const { return this->has(BF_CONT); }
    [[nodiscard]] inline bool is_weak() const { return this->has(BF_WEAK); }
    [[nodiscard]] inline bool has_byte() const { return this->has(BF_HASBYTE); }
    [[nodiscard]] inline bool has(u32 f) const { return (value & f) == f; }

    [[nodiscard]] inline u32 category() const {
        return value & (BF_UNKNOWN | BF_DATA | BF_CODE);
    }

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
        value &= ~BF_MBYTE;
        this->set(byte | BF_HASBYTE);
    }
};

static_assert(sizeof(Byte) == sizeof(u32));
static_assert(sizeof(Byte) == sizeof(RDByte));

} // namespace redasm
