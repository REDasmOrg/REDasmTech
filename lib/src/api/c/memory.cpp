#include "../internal/memory.h"
#include <redasm/memory.h>

namespace {

template<typename T, typename F>
T rd_getvalue(RDAddress address, bool* ok, F getter) {
    auto res = getter(address);
    if(ok) *ok = res.has_value();
    return res.value_or(T{});
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

bool rd_getbool(RDAddress address, bool* ok) {
    return rd_getvalue<bool>(address, ok, redasm::api::internal::get_bool);
}

char rd_getchar(RDAddress address, bool* ok) {
    return rd_getvalue<char>(address, ok, redasm::api::internal::get_char);
}

u8 rd_getu8(RDAddress address, bool* ok) {
    return rd_getvalue<u8>(address, ok, redasm::api::internal::get_u8);
}

u16 rd_getu16(RDAddress address, bool* ok) {
    return rd_getvalue<u16>(address, ok, redasm::api::internal::get_u16);
}

u32 rd_getu32(RDAddress address, bool* ok) {
    return rd_getvalue<u32>(address, ok, redasm::api::internal::get_u32);
}

u64 rd_getu64(RDAddress address, bool* ok) {
    return rd_getvalue<u64>(address, ok, redasm::api::internal::get_u64);
}

i8 rd_geti8(RDAddress address, bool* ok) {
    return rd_getvalue<i8>(address, ok, redasm::api::internal::get_i8);
}

i16 rd_geti16(RDAddress address, bool* ok) {
    return rd_getvalue<i16>(address, ok, redasm::api::internal::get_i16);
}

i32 rd_geti32(RDAddress address, bool* ok) {
    return rd_getvalue<i32>(address, ok, redasm::api::internal::get_i32);
}

i64 rd_geti64(RDAddress address, bool* ok) {
    return rd_getvalue<i64>(address, ok, redasm::api::internal::get_i64);
}

u16 rd_getu16be(RDAddress address, bool* ok) {
    return rd_getvalue<u16>(address, ok, redasm::api::internal::get_u16be);
}

u32 rd_getu32be(RDAddress address, bool* ok) {
    return rd_getvalue<u32>(address, ok, redasm::api::internal::get_u32be);
}

u64 rd_getu64be(RDAddress address, bool* ok) {
    return rd_getvalue<u64>(address, ok, redasm::api::internal::get_u64be);
}

i16 rd_geti16be(RDAddress address, bool* ok) {
    return rd_getvalue<i16>(address, ok, redasm::api::internal::get_i16be);
}

i32 rd_geti32be(RDAddress address, bool* ok) {
    return rd_getvalue<i32>(address, ok, redasm::api::internal::get_i32be);
}

i64 rd_geti64be(RDAddress address, bool* ok) {
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
