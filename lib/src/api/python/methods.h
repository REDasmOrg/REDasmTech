#pragma once

#include "common.h"
#include "redasm.h"
#include "typing.h"
#include <Python.h>

namespace redasm::api::python {

// clang-format off
inline PyMethodDef methods[] = {
    // redasm.h
    {"init", python::init, METH_NOARGS, nullptr},
    {"deinit", python::deinit, METH_NOARGS, nullptr},
    {"set_loglevel", python::set_loglevel, METH_O, nullptr},
    {"symbolize", python::symbolize, METH_O, nullptr},
    {"is_address", python::is_address, METH_O, nullptr},
    {"address_to_offset", python::address_to_offset, METH_O, nullptr},
    {"offset_to_address", python::offset_to_address, METH_O, nullptr},
    {"index_to_address", python::index_to_address, METH_O, nullptr},
    {"address_to_index", python::address_to_index, METH_O, nullptr},
    {"check_string", python::check_string, METH_O, nullptr},
    {"to_hex", python::to_cfunction(python::to_hex), METH_VARARGS | METH_KEYWORDS, nullptr},
    {"enqueue", python::enqueue, METH_O, nullptr},
    {"schedule", python::schedule, METH_O, nullptr},
    {"is_alnum", python::is_alnum, METH_O, nullptr},
    {"is_alpha", python::is_alpha, METH_O, nullptr},
    {"is_lower", python::is_lower, METH_O, nullptr},
    {"is_upper", python::is_upper, METH_O, nullptr},
    {"is_digit", python::is_digit, METH_O, nullptr},
    {"is_xdigit", python::is_xdigit, METH_O, nullptr},
    {"is_cntrl", python::is_cntrl, METH_O, nullptr},
    {"is_graph", python::is_graph, METH_O, nullptr},
    {"is_space", python::is_space, METH_O, nullptr},
    {"is_blank", python::is_blank, METH_O, nullptr},
    {"is_print", python::is_print, METH_O, nullptr},
    {"is_punct", python::is_punct, METH_O, nullptr},
    {"memory_info", python::memory_info, METH_NOARGS, nullptr},
    {"map_segment", python::map_segment, METH_VARARGS, nullptr},
    {"map_segment_n", python::map_segment_n, METH_VARARGS, nullptr},
    {"set_type", python::set_type, METH_VARARGS, nullptr},
    {"set_type_as", python::set_type_as, METH_VARARGS, nullptr},
    {"set_name", python::set_name, METH_VARARGS, nullptr},
    {"set_export", python::set_export, METH_O, nullptr},
    {"set_import", python::set_import, METH_O, nullptr},
    {"set_function", python::set_function, METH_O, nullptr},
    {"set_function_as", python::set_function_as, METH_VARARGS, nullptr},
    {"set_processor", python::set_processor, METH_O, nullptr},
    {"register_loader", python::to_cfunction(python::register_loader), METH_VARARGS | METH_KEYWORDS, nullptr},
    {"register_processor", python::to_cfunction(python::register_processor), METH_VARARGS | METH_KEYWORDS, nullptr},
    {"register_analyzer", python::to_cfunction(python::register_analyzer), METH_VARARGS | METH_KEYWORDS, nullptr},
    {"get_bool", python::get_bool, METH_O, nullptr},
    {"get_char", python::get_char, METH_O, nullptr},
    {"get_u8", python::get_u8, METH_O, nullptr},
    {"get_u16", python::get_u16, METH_O, nullptr},
    {"get_u32", python::get_u32, METH_O, nullptr},
    {"get_u64", python::get_u64, METH_O, nullptr},
    {"get_i8", python::get_i8, METH_O, nullptr},
    {"get_i16", python::get_i16, METH_O, nullptr},
    {"get_i32", python::get_i32, METH_O, nullptr},
    {"get_i64", python::get_i64, METH_O, nullptr},
    {"get_u16be", python::get_u16be, METH_O, nullptr},
    {"get_u32be", python::get_u32be, METH_O, nullptr},
    {"get_u64be", python::get_u64be, METH_O, nullptr},
    {"get_i16be", python::get_i16be, METH_O, nullptr},
    {"get_i32be", python::get_i32be, METH_O, nullptr},
    {"get_i64be", python::get_i64be, METH_O, nullptr},
    {"get_stringz", python::get_stringz, METH_O, nullptr},
    {"get_string", python::get_string, METH_VARARGS, nullptr},
    {"get_type", python::get_type, METH_VARARGS, nullptr},

    // typing.h
    {"size_of", python::size_of, METH_O, nullptr},
    {"create_struct", python::create_struct, METH_VARARGS, nullptr},

    {nullptr},
};
// clang-format on

} // namespace redasm::api::python
