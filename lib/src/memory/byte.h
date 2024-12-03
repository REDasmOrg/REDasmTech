#pragma once

#include "../error.h"
#include <redasm/byte.h>
#include <redasm/types.h>

namespace redasm {

struct Byte {
    u32 value;

    [[nodiscard]] bool is_unknown() const {
        return (value & BF_MUNKN) == BF_UNKNOWN;
    }

    [[nodiscard]] bool is_code() const { return (value & BF_CODE) == BF_CODE; }
    [[nodiscard]] bool is_data() const { return (value & BF_DATA) == BF_DATA; }
    [[nodiscard]] bool is_cont() const { return this->has(BF_CONT); }
    [[nodiscard]] bool has_byte() const { return this->has(BF_BYTE); }
    [[nodiscard]] bool has(u32 f) const { return (value & f) == f; }

    [[nodiscard]] u32 category() const {
        return value & (BF_UNKNOWN | BF_DATA | BF_CODE);
    }

    [[nodiscard]] u8 byte() const {
        assume(this->has_byte());
        return static_cast<u8>(value & 0xFF);
    }

    void set(u32 f) { this->set_flag(f, true); }
    void unset(u32 f) { this->set_flag(f, false); }

    void set_flag(u32 f, bool b) {
        if(b)
            value |= f;
        else
            value &= ~f;
    }

    void set_byte(u8 byte) {
        value &= ~BF_MBYTE;
        this->set(byte | BF_BYTE);
    }
};

static_assert(sizeof(Byte) == sizeof(u32));
static_assert(sizeof(Byte) == sizeof(RDByte));

} // namespace redasm
