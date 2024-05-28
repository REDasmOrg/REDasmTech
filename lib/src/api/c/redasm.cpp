#include "../internal/redasm.h"
#include "../../utils/object.h"
#include "../internal/memory.h"
#include <redasm/redasm.h>

bool rd_init(void) { return redasm::api::internal::init(); }
void rd_deinit(void) { redasm::api::internal::deinit(); }

void rd_free(void* obj) {
    redasm::api::internal::free(reinterpret_cast<redasm::Object*>(obj));
}

void rd_setloglevel(RDLogLevel l) { redasm::api::internal::set_loglevel(l); }

usize rd_getsegments(const RDSegment** segments) {
    return redasm::api::internal::get_segments(segments);
}

usize rd_getbytes(const RDByte** bytes) {
    return redasm::api::internal::get_bytes(bytes);
}

usize rd_memoryread(RDAddress address, char* data, usize n) {
    return redasm::api::internal::memory_read(address, data, n);
}

usize rd_getbits() { return redasm::api::internal::get_bits(); }

bool rd_checkstring(RDAddress address, RDStringResult* r) {
    return redasm::api::internal::check_string(address, r);
}

void rd_memoryinfo(RDMemoryInfo* mi) {
    return redasm::api::internal::memory_info(mi);
}

void rd_addsearchpath(const char* path) {
    if(path)
        redasm::api::internal::add_search_path(path);
}

RDBuffer* rd_loadfile(const char* filepath) {
    if(!filepath)
        return nullptr;

    return redasm::api::internal::load_file(filepath);
}

usize rd_test(RDBuffer* buffer, const RDTestResult** result) {
    static std::vector<RDTestResult> res;
    res = redasm::api::internal::test(buffer);

    if(result)
        *result = res.data();

    return res.size();
}

void rd_enqueue(RDAddress address) { redasm::api::internal::enqueue(address); }

void rd_schedule(RDAddress address) {
    redasm::api::internal::schedule(address);
}

void rd_disassemble() { redasm::api::internal::disassemble(); }
void rd_select(RDContext* context) { redasm::api::internal::select(context); }
bool rd_destroy(void) { return redasm::api::internal::destroy(); }
void rd_discard(void) { redasm::api::internal::discard(); }

const char* rd_getname(RDAddress address) {
    static std::string res;

    res = redasm::api::internal::get_name(address);
    if(res.empty())
        return nullptr;

    return res.c_str();
}

const char* rd_addresstosegment(RDAddress address) {
    std::string_view s = redasm::api::internal::address_to_segment(address);
    return s.empty() ? nullptr : s.data();
}

const char* rd_offsettosegment(RDOffset offset) {
    std::string_view s = redasm::api::internal::offset_to_segment(offset);
    return s.empty() ? nullptr : s.data();
}

bool rd_addresstoffset(RDAddress address, RDOffset* offset) {
    auto v = redasm::api::internal::address_to_offset(address);
    if(!v)
        return false;

    if(offset)
        *offset = *v;
    return true;
}

bool rd_offsettoaddress(RDOffset offset, RDAddress* address) {
    auto v = redasm::api::internal::offset_to_address(offset);
    if(!v)
        return false;

    if(address)
        *address = *v;
    return true;
}

bool rd_isaddress(RDAddress address) {
    return redasm::api::internal::is_address(address);
}

const char* rd_tohex_n(usize val, usize n) {
    static std::string res;
    res = redasm::api::internal::to_hex_n(val, n);
    return res.empty() ? nullptr : res.c_str();
}

const char* rd_tohex(usize val) {
    static std::string res;
    res = redasm::api::internal::to_hex(val);
    return res.empty() ? nullptr : res.c_str();
}
