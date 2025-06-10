#include "leb128.h"

namespace redasm::utils {

tl::optional<RDLEB128> decode_uleb128(const RDBuffer* buffer, usize idx) {
    if(!buffer || !buffer->length) return tl::nullopt;
    u64 val = 0;
    int shift = 0;

    for(usize i = idx; i < buffer->length; ++i) {
        u8 b;
        if(!buffer->get_byte(buffer, i, &b)) return tl::nullopt;

        val |= (static_cast<u64>(b & 0x7F) << shift);
        shift += 7;

        // Check if the continuation bit is set
        if((b & 0x80) == 0) {
            return RDLEB128{
                .size = i - idx + 1,
                .u_val = val,
            };
        }

        // If we have read too many bytes, return nullopt
        if(i == 9) {
            // ULEB128 can only encode 64 bits
            // So if we read 10 bytes, it's an error
            return tl::nullopt;
        }
    }

    // If we reach here, we didn't find a terminating byte
    return tl::nullopt;
}

tl::optional<RDLEB128> decode_leb128(const RDBuffer* buffer, usize idx) {
    if(!buffer || !buffer->length) return tl::nullopt;

    i64 val = 0;
    int shift = 0;

    for(usize i = idx; i < buffer->length; ++i) {
        u8 b;
        if(!buffer->get_byte(buffer, i, &b)) return tl::nullopt;
        val |= (static_cast<i64>(b & 0x7F) << shift);
        shift += 7;

        // Check if the continuation bit is set
        if((b & 0x80) == 0) {
            // If the number is negative and the sign bit is set,
            // we need to extend the sign
            if(shift < 64 && (b & 0x40) != 0) val |= (-1LL << shift);

            return RDLEB128{
                .size = i - idx + 1,
                .i_val = val,
            };
        }

        // If we have read too many bytes, return nullopt
        if(i == 9) {
            // LEB128 can only encode 64 bits, so if we read 10 bytes,
            // it's an error
            return tl::nullopt;
        }
    }

    // If we reach here, we didn't find a terminating byte
    return tl::nullopt;
}

} // namespace redasm::utils
