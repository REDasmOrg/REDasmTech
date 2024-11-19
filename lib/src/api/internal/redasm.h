#pragma once

#include "../../typing/typing.h"
#include "../../utils/object.h"
#include <redasm/redasm.h>
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
void log(std::string_view s);
void status(std::string_view s);
std::string symbolize(std::string s);

void add_search_path(const std::string& p);
RDBuffer* load_file(const std::string& filepath);
std::vector<RDTestResult> test(RDBuffer* buffer);
void select(RDContext* context);
void free(Object* obj);
bool destroy();
void discard();
void enqueue(RDAddress address);
void schedule(RDAddress address);
void disassemble();
bool tick(const RDAnalysisStatus** s);
std::string to_hex_n(usize v, usize n);
std::string to_hex(usize v);
tl::optional<RDAddress> get_ep();

usize get_segments(const RDSegment** segments);
bool find_segment(RDAddress address, RDSegment* segment);
std::vector<RDRef> get_refs(RDAddress address);
usize get_bytes(const RDByte** bytes);
void set_bits(int bits);
int get_bits();

bool map_segment(const std::string& name, RDAddress address,
                 RDAddress endaddress, RDOffset offset, RDOffset endoffset,
                 usize type);
bool map_segment_n(const std::string& name, RDAddress address, usize asize,
                   RDOffset offset, usize osize, usize type);

tl::optional<RDAddress> get_address(std::string_view name);
std::string get_comment(RDAddress address);
std::string get_name(RDAddress address);
std::string render_text(RDAddress address);

tl::optional<typing::Value> set_type(RDAddress address, std::string_view tname);
tl::optional<typing::Value> set_type_as(RDAddress address,
                                        std::string_view tname,
                                        const std::string& dbname);

bool set_comment(RDAddress address, std::string_view comment);
bool set_name(RDAddress address, const std::string& name);
bool set_export(RDAddress address);
bool set_import(RDAddress address);
bool set_function_as(RDAddress address, const std::string& name);
bool set_function(RDAddress address);
bool set_entry(RDAddress address, const std::string& name);

bool is_address(RDAddress address);
bool address_to_segment(RDAddress address, RDSegment* res);
bool offset_to_segment(RDOffset offset, RDSegment* res);
tl::optional<RDOffset> address_to_offset(RDAddress address);
tl::optional<RDAddress> offset_to_address(RDOffset offset);
tl::optional<usize> address_to_index(RDAddress address);
tl::optional<RDAddress> index_to_address(usize idx);

tl::optional<bool> get_bool(RDAddress address);
tl::optional<char> get_char(RDAddress address);
tl::optional<u8> get_u8(RDAddress address);
tl::optional<u16> get_u16(RDAddress address);
tl::optional<u32> get_u32(RDAddress address);
tl::optional<u64> get_u64(RDAddress address);
tl::optional<i8> get_i8(RDAddress address);
tl::optional<i16> get_i16(RDAddress address);
tl::optional<i32> get_i32(RDAddress address);
tl::optional<i64> get_i64(RDAddress address);
tl::optional<u16> get_u16be(RDAddress address);
tl::optional<u32> get_u32be(RDAddress address);
tl::optional<u64> get_u64be(RDAddress address);
tl::optional<i16> get_i16be(RDAddress address);
tl::optional<i32> get_i32be(RDAddress address);
tl::optional<i64> get_i64be(RDAddress address);
tl::optional<std::string> get_stringz(RDAddress address);
tl::optional<std::string> get_string(RDAddress address, usize n);
tl::optional<typing::Value> get_type(RDAddress address, std::string_view tname);

} // namespace redasm::api::internal
