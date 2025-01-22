#pragma once

#include "../../plugins/modulemanager.h"
#include "../../typing/base.h"
#include "../../typing/value.h"
#include <redasm/redasm.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm::api::internal {

bool is_alnum(u8 b);
bool is_alpha(u8 b);
bool is_lower(u8 b);
bool is_upper(u8 b);
bool is_digit(u8 b);
bool is_xdigit(u8 b);
bool is_cntrl(u8 b);
bool is_graph(u8 b);
bool is_space(u8 b);
bool is_blank(u8 b);
bool is_print(u8 b);
bool is_punct(u8 b);

bool init(const RDInitParams* params);
void deinit();
void set_loglevel(RDLogLevel l);
void add_searchpath(const std::string& sp);
void set_userdata(const std::string& k, uptr v);
tl::optional<uptr> get_userdata(const std::string& k);
const mm::SearchPaths& get_searchpaths();
void log(std::string_view s);
void status(std::string_view s);
std::string symbolize(std::string s);

RDBuffer* load_file(const std::string& filepath);
std::vector<RDTestResult> test(RDBuffer* buffer);
void select(const RDTestResult* tr);
void free(void* obj);
bool destroy();
void discard();
void enqueue(RDAddress address);
void schedule(RDAddress address);
void disassemble();
bool tick(const RDWorkerStatus** s);
std::vector<RDAddress> get_entries();
std::vector<RDProblem> get_problems();

std::vector<RDRef> get_refsfrom(RDAddress fromaddr);
std::vector<RDRef> get_refsfromtype(RDAddress fromaddr, usize type);
std::vector<RDRef> get_refsto(RDAddress toaddr);
std::vector<RDRef> get_refstotype(RDAddress toaddr, usize type);
usize get_memory(const RDByte** bytes);
usize get_file(const u8** bytes);

tl::optional<RDAddress> get_address(std::string_view name);
std::string get_comment(RDAddress address);
std::string get_name(RDAddress address);
std::string render_text(RDAddress address);

tl::optional<typing::Value> map_type(RDAddress address, std::string_view tname);

tl::optional<typing::Value> set_type_ex(RDAddress address, const RDType* t,
                                        usize flags);

tl::optional<typing::Value>
set_typename_ex(RDAddress address, typing::FullTypeName tname, usize flags);

tl::optional<typing::Value> get_type(RDAddress address, const RDType* t);
tl::optional<typing::Value> get_typename(RDAddress address,
                                         std::string_view tname);

tl::optional<typing::Value> map_type_ex(RDOffset offset, RDAddress address,
                                        const RDType* t, usize flags);

tl::optional<typing::Value> map_typename_ex(RDOffset offset, RDAddress address,
                                            typing::FullTypeName tname,
                                            usize flags);

bool set_comment(RDAddress address, std::string_view comment);
bool set_name_ex(RDAddress address, const std::string& name, usize flags);
bool set_function_ex(RDAddress address, usize flags);
bool set_entry(RDAddress address, const std::string& name);

usize get_segments(const RDSegment** segments);
bool find_segment(RDAddress address, RDSegment* segment);

bool map_segment(const std::string& name, RDAddress address,
                 RDAddress endaddress, RDOffset offset, RDOffset endoffset,
                 usize type);
bool map_segment_n(const std::string& name, RDAddress address, usize asize,
                   RDOffset offset, usize osize, usize type);

void add_ref(RDAddress fromaddr, RDAddress toaddr, usize type);
void add_problem(RDAddress address, const std::string& problem);
bool address_to_segment(RDAddress address, RDSegment* res);
bool offset_to_segment(RDOffset offset, RDSegment* res);
tl::optional<RDOffset> to_offset(RDAddress address);
tl::optional<RDAddress> to_address(RDOffset offset);
tl::optional<RDAddress> from_reladdress(RDAddress reladdress);
tl::optional<usize> address_to_index(RDAddress address);
tl::optional<RDAddress> index_to_address(usize idx);

inline bool set_name(RDAddress address, const std::string& name) {
    return api::internal::set_name_ex(address, name, 0);
}

inline bool set_function(RDAddress address) {
    return api::internal::set_function_ex(address, 0);
}

inline tl::optional<typing::Value> set_type(RDAddress address,
                                            const RDType* t) {
    return api::internal::set_type_ex(address, t, 0);
}

inline tl::optional<typing::Value> set_typename(RDAddress address,
                                                typing::FullTypeName tname) {
    return api::internal::set_typename_ex(address, tname, 0);
}

inline tl::optional<typing::Value> map_type(RDOffset offset, RDAddress address,
                                            const RDType* t) {
    return api::internal::map_type_ex(offset, address, t, 0);
}

inline tl::optional<typing::Value>
map_typename(RDOffset offset, RDAddress address, typing::FullTypeName tname) {
    return api::internal::map_typename_ex(offset, address, tname, 0);
}

} // namespace redasm::api::internal
