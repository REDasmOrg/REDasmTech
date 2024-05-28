#pragma once

#include "../error.h"
#include <redasm/byte.h>
#include <redasm/types.h>

namespace redasm {

struct Byte {
    u32 flags;

    [[nodiscard]] inline bool is_unknown() const {
        return (flags & BF_UDCMASK) == BF_UNKNOWN;
    }

    [[nodiscard]] inline bool is_code() const {
        return (flags & BF_UDCMASK) == BF_CODE;
    }

    [[nodiscard]] inline bool is_data() const {
        return (flags & BF_UDCMASK) == BF_DATA;
    }

    [[nodiscard]] inline bool is_cont() const { return this->has(BF_CONT); }
    [[nodiscard]] inline bool has_byte() const { return this->has(BF_HASBYTE); }
    [[nodiscard]] inline bool has(u32 f) const { return flags & f; }

    [[nodiscard]] inline u8 byte() const {
        assume(this->has_byte());
        return static_cast<u8>(flags & 0xFF);
    }

    inline void set(u32 f) { this->set_flag(f, true); }
    inline void unset(u32 f) { this->set_flag(f, false); }

    inline void set_flag(u32 f, bool b) {
        if(b)
            flags |= f;
        else
            flags &= ~f;
    }

    inline void set_byte(u8 byte) {
        flags &= ~BF_BYTEMASK;
        this->set(byte | BF_HASBYTE);
    }

    inline void mark_data() {
        flags &= ~BF_UDCMASK;
        flags |= BF_DATA;
    }

    inline void mark_code() {
        flags &= ~BF_UDCMASK;
        flags |= BF_CODE;
    }

    inline void as_unknown() { flags &= ~BF_UDCMASK; }
};

static_assert(sizeof(Byte) == sizeof(u32));
static_assert(sizeof(Byte) == sizeof(RDByte));

} // namespace redasm
