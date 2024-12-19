#include "../internal/redasm.h"
#include "../internal/graph.h"
#include "../marshal.h"
#include <algorithm>
#include <redasm/redasm.h>

namespace {

template<typename T, typename F>
bool rd_getvalue(RDAddress address, T* v, F getter) {
    auto res = getter(address);
    res.map([&](T x) { *v = x; });
    return res.has_value();
}

} // namespace

bool rd_init(const RDInitParams* params) {
    return redasm::api::internal::init(params);
}

void rd_deinit(void) { redasm::api::internal::deinit(); }
void rd_free(void* obj) { redasm::api::internal::free(obj); }
void rd_setloglevel(RDLogLevel l) { redasm::api::internal::set_loglevel(l); }

void rd_addsearchpath(const char* sp) {
    if(sp)
        redasm::api::internal::add_searchpath(sp);
}

usize rd_getsearchpaths(const char*** spaths) {
    static std::vector<const char*> res;

    const auto& sp = redasm::api::internal::get_searchpaths();
    res.resize(sp.size());

    std::ranges::transform(sp, res.begin(),
                           [](const std::string& x) { return x.c_str(); });

    if(spaths)
        *spaths = res.data();

    return res.size();
}

void rd_setuserdata(const char* k, uptr v) {
    if(k)
        redasm::api::internal::set_userdata(k, v);
}

bool rd_getuserdata(const char* k, uptr* v) {
    if(!k)
        return false;

    auto ud = redasm::api::internal::get_userdata(k);

    ud.map([&](uptr x) {
        if(v)
            *v = x;
    });

    return ud.has_value();
}

void rd_log(const char* s) {
    if(s)
        redasm::api::internal::log(s);
}

void rd_status(const char* s) {
    if(s)
        redasm::api::internal::status(s);
}

bool rd_gettype(RDAddress address, const char* tname, RDValue* v) {
    static redasm::typing::Value res;

    if(tname) {
        return redasm::api::internal::get_type(address, tname)
            .map_or(
                [&](const redasm::typing::Value& x) {
                    res = x;
                    if(v)
                        *redasm::api::from_c(v) = x;
                    return true;
                },
                false);
    }

    return false;
}

usize rd_getentries(RDAddress** entries) {
    static std::vector<RDAddress> res;
    res = redasm::api::internal::get_entries();

    if(entries)
        *entries = res.data();

    return res.size();
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

const char* rd_rendertext(RDAddress address) {
    static std::string s;
    s = redasm::api::internal::render_text(address);
    return s.c_str();
}

bool rd_mapsegment(const char* name, RDAddress address, RDAddress endaddress,
                   RDOffset offset, RDOffset endoffset, usize type) {

    if(name)
        return redasm::api::internal::map_segment(name, address, endaddress,
                                                  offset, endoffset, type);

    return false;
}

bool rd_mapsegment_n(const char* name, RDAddress address, usize asize,
                     RDOffset offset, usize osize, usize type) {
    if(name)
        return redasm::api::internal::map_segment_n(name, address, asize,
                                                    offset, osize, type);

    return false;
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

void rd_disassemble(void) { redasm::api::internal::disassemble(); }
void rd_select(RDContext* context) { redasm::api::internal::select(context); }
bool rd_destroy(void) { return redasm::api::internal::destroy(); }
void rd_discard(void) { redasm::api::internal::discard(); }

usize rd_getproblems(const RDProblem** problems) {
    static std::vector<RDProblem> res;
    res = redasm::api::internal::get_problems();
    if(problems)
        *problems = res.data();
    return res.size();
}

bool rd_setcomment(RDAddress address, const char* comment) {
    if(comment)
        return redasm::api::internal::set_comment(address, comment);

    return false;
}

bool rd_settype(RDAddress address, const RDType* type) {
    return redasm::api::internal::set_type(address, type).has_value();
}

bool rd_settype_ex(RDAddress address, const RDType* type, usize flags) {
    return redasm::api::internal::set_type_ex(address, type, flags).has_value();
}

bool rd_settypename(RDAddress address, const char* tname) {
    if(tname)
        return redasm::api::internal::set_typename(address, tname).has_value();

    return false;
}

bool rd_settypename_ex(RDAddress address, const char* tname, usize flags) {
    if(tname)
        return redasm::api::internal::set_typename_ex(address, tname, flags)
            .has_value();

    return false;
}

bool rd_setfunction(RDAddress address) {
    return redasm::api::internal::set_function(address);
}

bool rd_setfunction_ex(RDAddress address, usize flags) {
    return redasm::api::internal::set_function_ex(address, flags);
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

bool rd_setname_ex(RDAddress address, const char* name, usize flags) {
    return name && redasm::api::internal::set_name_ex(address, name, flags);
}

void rd_addref(RDAddress fromaddr, RDAddress toaddr, usize type) {
    redasm::api::internal::add_ref(fromaddr, toaddr, type);
}

void rd_addproblem(RDAddress address, const char* problem) {
    if(problem)
        redasm::api::internal::add_problem(address, problem);
}

bool rd_getaddress(const char* name, RDAddress* address) {
    return name && redasm::api::internal::get_address(name).map_or(
                       [&](RDAddress x) {
                           if(address)
                               *address = x;
                           return true;
                       },
                       false);
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

usize rd_getrefsfrom(RDAddress fromaddr, const RDRef** refs) {
    static std::vector<RDRef> r;
    r = redasm::api::internal::get_refsfrom(fromaddr);

    if(refs)
        *refs = r.data();

    return r.size();
}

usize rd_getrefsfromtype(RDAddress fromaddr, usize type, const RDRef** refs) {
    static std::vector<RDRef> r;
    r = redasm::api::internal::get_refsfromtype(fromaddr, type);

    if(refs)
        *refs = r.data();

    return r.size();
}

usize rd_getrefsto(RDAddress toaddr, const RDRef** refs) {
    static std::vector<RDRef> r;
    r = redasm::api::internal::get_refsto(toaddr);

    if(refs)
        *refs = r.data();

    return r.size();
}

usize rd_getrefstotype(RDAddress toaddr, usize type, const RDRef** refs) {
    static std::vector<RDRef> r;
    r = redasm::api::internal::get_refstotype(toaddr, type);

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
    auto v = redasm::api::internal::to_offset(address);
    if(!v)
        return false;

    if(offset)
        *offset = *v;
    return true;
}

bool rd_offsettoaddress(RDOffset offset, RDAddress* address) {
    auto v = redasm::api::internal::to_address(offset);
    if(!v)
        return false;

    if(address)
        *address = *v;
    return true;
}

bool rd_isaddress(RDAddress address) {
    return redasm::api::internal::is_address(address);
}

bool rd_getbool(RDAddress address, bool* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_bool);
}

bool rd_getchar(RDAddress address, char* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_char);
}

bool rd_getu8(RDAddress address, u8* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_u8);
}

bool rd_getu16(RDAddress address, u16* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_u16);
}

bool rd_getu32(RDAddress address, u32* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_u32);
}

bool rd_getu64(RDAddress address, u64* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_u64);
}

bool rd_geti8(RDAddress address, i8* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_i8);
}

bool rd_geti16(RDAddress address, i16* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_i16);
}

bool rd_geti32(RDAddress address, i32* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_i32);
}

bool rd_geti64(RDAddress address, i64* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_i64);
}

bool rd_getu16be(RDAddress address, u16* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_u16be);
}

bool rd_getu32be(RDAddress address, u32* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_u32be);
}

bool rd_getu64be(RDAddress address, u64* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_u64be);
}

bool rd_geti16be(RDAddress address, i16* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_i16be);
}

bool rd_geti32be(RDAddress address, i32* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_i32be);
}

bool rd_geti64be(RDAddress address, i64* v) {
    return rd_getvalue(address, v, redasm::api::internal::get_i64be);
}
