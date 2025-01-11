#pragma once

#include "../utils/hash.h"
#include <redasm/typing.h>
#include <vector>

namespace redasm::typing {

namespace names {

inline constexpr const char* BOOL = "bool";
inline constexpr const char* CHAR = "char";
inline constexpr const char* WCHAR = "wchar";
inline constexpr const char* U8 = "u8";
inline constexpr const char* U16 = "u16";
inline constexpr const char* U32 = "u32";
inline constexpr const char* U64 = "u64";
inline constexpr const char* I8 = "i8";
inline constexpr const char* I16 = "i16";
inline constexpr const char* I32 = "i32";
inline constexpr const char* I64 = "i64";
inline constexpr const char* U16BE = "u16be";
inline constexpr const char* U32BE = "u32be";
inline constexpr const char* U64BE = "u64be";
inline constexpr const char* I16BE = "i16be";
inline constexpr const char* I32BE = "i32be";
inline constexpr const char* I64BE = "i64be";
inline constexpr const char* STR = "str";
inline constexpr const char* WSTR = "wstr";

} // namespace names

namespace ids {

inline constexpr u32 BOOL = hash::static_murmur3(typing::names::BOOL);
inline constexpr u32 CHAR = hash::static_murmur3(typing::names::CHAR);
inline constexpr u32 WCHAR = hash::static_murmur3(typing::names::WCHAR);
inline constexpr u32 U8 = hash::static_murmur3(typing::names::U8);
inline constexpr u32 U16 = hash::static_murmur3(typing::names::U16);
inline constexpr u32 U32 = hash::static_murmur3(typing::names::U32);
inline constexpr u32 U64 = hash::static_murmur3(typing::names::U64);
inline constexpr u32 I8 = hash::static_murmur3(typing::names::I8);
inline constexpr u32 I16 = hash::static_murmur3(typing::names::I16);
inline constexpr u32 I32 = hash::static_murmur3(typing::names::I32);
inline constexpr u32 I64 = hash::static_murmur3(typing::names::I64);
inline constexpr u32 U16BE = hash::static_murmur3(typing::names::U16BE);
inline constexpr u32 U32BE = hash::static_murmur3(typing::names::U32BE);
inline constexpr u32 U64BE = hash::static_murmur3(typing::names::U64BE);
inline constexpr u32 I16BE = hash::static_murmur3(typing::names::I16BE);
inline constexpr u32 I32BE = hash::static_murmur3(typing::names::I32BE);
inline constexpr u32 I64BE = hash::static_murmur3(typing::names::I64BE);
inline constexpr u32 STR = hash::static_murmur3(typing::names::STR);
inline constexpr u32 WSTR = hash::static_murmur3(typing::names::WSTR);

} // namespace ids

using FullTypeName = std::string_view;
using TypeName = std::string_view;
using StructField = std::pair<std::string, std::string>;
using Struct = std::vector<typing::StructField>;

inline TypeId type_id(TypeName tn) { return hash::murmur3(tn); }

} // namespace redasm::typing
