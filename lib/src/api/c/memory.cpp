#include "../internal/memory.h"
#include <redasm/memory.h>

namespace {

template<typename T, typename F>
bool rd_getvalue(RDAddress address, T* v, F getter) {
    auto res = getter(address);
    res.map([&](const T& x) {
        if(v) *v = x;
    });
    return res.has_value();
}

} // namespace

void rd_memoryinfo(RDMemoryInfo* mi) { redasm::api::internal::memory_info(mi); }

bool rd_map(RDAddress startaddr, RDAddress endaddr) {
    return redasm::api::internal::memory_map(startaddr, endaddr);
}

bool rd_map_n(RDAddress baseaddress, usize size) {
    return redasm::api::internal::memory_map_n(baseaddress, size);
}

usize rd_read(RDAddress address, void* data, usize n) {
    return redasm::api::internal::read(address, data, n);
}

bool rd_getbool(RDAddress address, bool* v) {
    return rd_getvalue<bool>(address, v, redasm::api::internal::get_bool);
}

bool rd_getchar(RDAddress address, char* ok) {
    return rd_getvalue<char>(address, ok, redasm::api::internal::get_char);
}

bool rd_getu8(RDAddress address, u8* ok) {
    return rd_getvalue<u8>(address, ok, redasm::api::internal::get_u8);
}

bool rd_getu16(RDAddress address, u16* ok) {
    return rd_getvalue<u16>(address, ok, redasm::api::internal::get_u16);
}

bool rd_getu32(RDAddress address, u32* ok) {
    return rd_getvalue<u32>(address, ok, redasm::api::internal::get_u32);
}

bool rd_getu64(RDAddress address, u64* ok) {
    return rd_getvalue<u64>(address, ok, redasm::api::internal::get_u64);
}

bool rd_geti8(RDAddress address, i8* ok) {
    return rd_getvalue<i8>(address, ok, redasm::api::internal::get_i8);
}

bool rd_geti16(RDAddress address, i16* ok) {
    return rd_getvalue<i16>(address, ok, redasm::api::internal::get_i16);
}

bool rd_geti32(RDAddress address, i32* ok) {
    return rd_getvalue<i32>(address, ok, redasm::api::internal::get_i32);
}

bool rd_geti64(RDAddress address, i64* ok) {
    return rd_getvalue<i64>(address, ok, redasm::api::internal::get_i64);
}

bool rd_getu16be(RDAddress address, u16* ok) {
    return rd_getvalue<u16>(address, ok, redasm::api::internal::get_u16be);
}

bool rd_getu32be(RDAddress address, u32* ok) {
    return rd_getvalue<u32>(address, ok, redasm::api::internal::get_u32be);
}

bool rd_getu64be(RDAddress address, u64* ok) {
    return rd_getvalue<u64>(address, ok, redasm::api::internal::get_u64be);
}

bool rd_geti16be(RDAddress address, i16* ok) {
    return rd_getvalue<i16>(address, ok, redasm::api::internal::get_i16be);
}

bool rd_geti32be(RDAddress address, i32* ok) {
    return rd_getvalue<i32>(address, ok, redasm::api::internal::get_i32be);
}

bool rd_geti64be(RDAddress address, i64* ok) {
    return rd_getvalue<i64>(address, ok, redasm::api::internal::get_i64be);
}

const char* rd_getstr(RDAddress address, usize n) {
    static std::string res;
    auto str = redasm::api::internal::get_str(address, n);

    if(str) {
        res = *str;
        return res.c_str();
    }

    return nullptr;
}

const char* rd_getwstr(RDAddress address, usize n) {
    static std::string res;
    auto str = redasm::api::internal::get_wstr(address, n);

    if(str) {
        res = *str;
        return res.c_str();
    }

    return nullptr;
}

const char* rd_getstrz(RDAddress address) {
    static std::string res;
    auto str = redasm::api::internal::get_strz(address);

    if(str) {
        res = *str;
        return res.c_str();
    }

    return nullptr;
}

const char* rd_getwstrz(RDAddress address) {
    static std::string res;
    auto str = redasm::api::internal::get_wstrz(address);

    if(str) {
        res = *str;
        return res.c_str();
    }

    return nullptr;
}

bool rd_isaddress(RDAddress address) {
    return redasm::api::internal::is_address(address);
}
