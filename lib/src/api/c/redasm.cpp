#include "../internal/redasm.h"
#include "../../utils/object.h"
#include "../internal/graph.h"
#include "../internal/memory.h"
#include "../marshal.h"
#include <redasm/redasm.h>

bool rd_init(void) { return redasm::api::internal::init(); }
void rd_deinit(void) { redasm::api::internal::deinit(); }

void rd_free(void* obj) {
    redasm::api::internal::free(reinterpret_cast<redasm::Object*>(obj));
}

void rd_setloglevel(RDLogLevel l) { redasm::api::internal::set_loglevel(l); }

bool rd_gettype(RDAddress address, const char* tname, RDValue* v) {
    static redasm::typing::Value res;

    if(tname) {
        return redasm::api::internal::get_type(address, tname)
            .map_or(
                [&](const redasm::typing::Value& x) {
                    res = x;
                    if(v)
                        *v = redasm::api::to_c(res);
                    return true;
                },
                false);
    }

    return false;
}

bool rd_getep(RDAddress* ep) {
    return redasm::api::internal::get_ep().map_or(
        [&](RDAddress addr) {
            if(ep)
                *ep = addr;
            return true;
        },
        false);
}

bool rd_tick(const RDEngineStatus** s) {
    return redasm::api::internal::tick(s);
}

usize rd_getsegments(const RDSegment** segments) {
    return redasm::api::internal::get_segments(segments);
}

usize rd_getbytes(const RDByte** bytes) {
    return redasm::api::internal::get_bytes(bytes);
}

usize rd_memoryread(RDAddress address, char* data, usize n) {
    return redasm::api::internal::memory_read(address, data, n);
}

int rd_getbits() { return redasm::api::internal::get_bits(); }
void rd_setbits(int bits) { redasm::api::internal::set_bits(bits); }

bool rd_checkstring(RDAddress address, RDStringResult* r) {
    return redasm::api::internal::check_string(address, r);
}

void rd_memoryinfo(RDMemoryInfo* mi) { redasm::api::internal::memory_info(mi); }

const char* rd_rendertext(RDAddress address) {
    static std::string s;
    s = redasm::api::internal::render_text(address);
    return s.c_str();
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

bool rd_setcomment(RDAddress address, const char* comment) {
    if(comment)
        return redasm::api::internal::set_comment(address, comment);

    return false;
}

bool rd_settype(RDAddress address, const char* tname) {
    if(tname)
        return redasm::api::internal::set_type(address, tname).has_value();

    return false;
}

bool rd_setfunction(RDAddress address) {
    return redasm::api::internal::set_function(address);
}

bool rd_setfunctionas(RDAddress address, const char* name) {
    if(name)
        return redasm::api::internal::set_function_as(address, name);

    return false;
}

bool rd_setentry(RDAddress address, const char* name) {
    std::string n;
    if(name)
        n = name;

    return redasm::api::internal::set_entry(address, name);
}

bool rd_setname(RDAddress address, const char* name) {
    return name && redasm::api::internal::set_name(address, name);
}

bool rd_getaddress(const char* name, RDAddress* address) {
    return name && redasm::api::internal::get_address(name)
                       .map([&](RDAddress x) {
                           if(address)
                               *address = x;

                           return true;
                       })
                       .value_or(false);
}

const char* rd_getname(RDAddress address) {
    static std::string res;

    res = redasm::api::internal::get_name(address);
    if(res.empty())
        return nullptr;

    return res.c_str();
}

const char* rd_getcomment(RDAddress address) {
    static std::string res;

    res = redasm::api::internal::get_comment(address);
    if(res.empty())
        return nullptr;

    return res.c_str();
}

usize rd_getrefs(RDAddress address, const RDRef** refs) {
    static std::vector<RDRef> r;
    r = redasm::api::internal::get_refs(address);

    if(refs)
        *refs = r.data();

    return r.size();
}

bool rd_addresstosegment(RDAddress address, RDSegment* s) {
    return redasm::api::internal::address_to_segment(address, s);
}

bool rd_offsettosegment(RDOffset offset, RDSegment* s) {
    return redasm::api::internal::offset_to_segment(offset, s);
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
