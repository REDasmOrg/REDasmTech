#pragma once

#include <array>
#include <redasm/typing.h>

namespace redasm::typing::primitive {

// clang-format off
inline constexpr std::array<const char*, T_NPRIMITIVES> NAMES = {
    "bool",  
    "char", "wchar", 
    "u8", "u16", "u32", "u64",   
    "i8", "i16", "i32", "i64", 
    "u16be", "u32be", "u64be", 
    "i16be", "i32be", "i64be", 
    "str", "wstr",
    "uleb128", "leb128",
};

inline std::array<RDTypeDef, T_NPRIMITIVES> types = {{
    {.kind = TK_PRIMITIVE, .name = NAMES[0],  .size = sizeof(bool),     .flags = 0,                           .t_primitive = T_BOOL},
    {.kind = TK_PRIMITIVE, .name = NAMES[1],  .size = sizeof(char),     .flags = 0,                           .t_primitive = T_CHAR},
    {.kind = TK_PRIMITIVE, .name = NAMES[2],  .size = sizeof(char16_t), .flags = 0,                           .t_primitive = T_WCHAR},
    {.kind = TK_PRIMITIVE, .name = NAMES[3],  .size = sizeof(u8),       .flags = TF_INT,                      .t_primitive = T_U8},
    {.kind = TK_PRIMITIVE, .name = NAMES[4],  .size = sizeof(u16),      .flags = TF_INT,                      .t_primitive = T_U16},
    {.kind = TK_PRIMITIVE, .name = NAMES[5],  .size = sizeof(u32),      .flags = TF_INT,                      .t_primitive = T_U32},
    {.kind = TK_PRIMITIVE, .name = NAMES[6],  .size = sizeof(u64),      .flags = TF_INT,                      .t_primitive = T_U64},
    {.kind = TK_PRIMITIVE, .name = NAMES[7],  .size = sizeof(i8),       .flags = TF_INT | TF_SIGN,            .t_primitive = T_I8},
    {.kind = TK_PRIMITIVE, .name = NAMES[8],  .size = sizeof(i16),      .flags = TF_INT | TF_SIGN,            .t_primitive = T_I16},
    {.kind = TK_PRIMITIVE, .name = NAMES[9],  .size = sizeof(i32),      .flags = TF_INT | TF_SIGN,            .t_primitive = T_I32},
    {.kind = TK_PRIMITIVE, .name = NAMES[10], .size = sizeof(i64),      .flags = TF_INT | TF_SIGN,            .t_primitive = T_I64},
    {.kind = TK_PRIMITIVE, .name = NAMES[11], .size = sizeof(u16),      .flags = TF_INT | TF_BIG,             .t_primitive = T_U16BE},
    {.kind = TK_PRIMITIVE, .name = NAMES[12], .size = sizeof(u32),      .flags = TF_INT | TF_BIG,             .t_primitive = T_U32BE},
    {.kind = TK_PRIMITIVE, .name = NAMES[13], .size = sizeof(u64),      .flags = TF_INT | TF_BIG,             .t_primitive = T_U64BE},
    {.kind = TK_PRIMITIVE, .name = NAMES[14], .size = sizeof(i16),      .flags = TF_INT | TF_SIGN | TF_BIG,   .t_primitive = T_I16BE},
    {.kind = TK_PRIMITIVE, .name = NAMES[15], .size = sizeof(i32),      .flags = TF_INT | TF_SIGN | TF_BIG,   .t_primitive = T_I32BE},
    {.kind = TK_PRIMITIVE, .name = NAMES[16], .size = sizeof(i64),      .flags = TF_INT | TF_SIGN | TF_BIG,   .t_primitive = T_I64BE},
    {.kind = TK_PRIMITIVE, .name = NAMES[17], .size = sizeof(char),     .flags = TF_VAR,                      .t_primitive = T_STR},
    {.kind = TK_PRIMITIVE, .name = NAMES[18], .size = sizeof(char16_t), .flags = TF_VAR,                      .t_primitive = T_WSTR},
    {.kind = TK_PRIMITIVE, .name = NAMES[19], .size = 0,                .flags = TF_INT | TF_SIGN | TF_VAR,   .t_primitive = T_ULEB128},
    {.kind = TK_PRIMITIVE, .name = NAMES[20], .size = 0,                .flags = TF_INT | TF_SIGN | TF_VAR,   .t_primitive = T_LEB128},
}};
// clang-format on

} // namespace redasm::typing::primitive
