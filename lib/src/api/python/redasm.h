#pragma once

#include <Python.h>

namespace redasm::api::python {

PyObject* is_alnum(PyObject* self, PyObject* args);
PyObject* is_alpha(PyObject* self, PyObject* args);
PyObject* is_lower(PyObject* self, PyObject* args);
PyObject* is_upper(PyObject* self, PyObject* args);
PyObject* is_digit(PyObject* self, PyObject* args);
PyObject* is_xdigit(PyObject* self, PyObject* args);
PyObject* is_cntrl(PyObject* self, PyObject* args);
PyObject* is_graph(PyObject* self, PyObject* args);
PyObject* is_space(PyObject* self, PyObject* args);
PyObject* is_blank(PyObject* self, PyObject* args);
PyObject* is_print(PyObject* self, PyObject* args);
PyObject* is_punct(PyObject* self, PyObject* args);

PyObject* init(PyObject* self, PyObject* args);
PyObject* deinit(PyObject* self, PyObject* args);
PyObject* log(PyObject* self, PyObject* args);
PyObject* status(PyObject* self, PyObject* args);
PyObject* set_loglevel(PyObject* self, PyObject* args);
PyObject* add_searchpath(PyObject* self, PyObject* args);
PyObject* get_searchpaths(PyObject* self, PyObject* args);
PyObject* get_problems(PyObject* self, PyObject* args);
PyObject* set_userdata(PyObject* self, PyObject* args);
PyObject* get_userdata(PyObject* self, PyObject* args);
PyObject* symbolize(PyObject* self, PyObject* args);
PyObject* is_address(PyObject* self, PyObject* args);
PyObject* to_offset(PyObject* self, PyObject* args);
PyObject* to_address(PyObject* self, PyObject* args);
PyObject* from_reladdress(PyObject* self, PyObject* args);
PyObject* address_to_index(PyObject* self, PyObject* args);
PyObject* index_to_address(PyObject* self, PyObject* args);
PyObject* check_string(PyObject* self, PyObject* args);

PyObject* get_entries(PyObject* self, PyObject* args);
PyObject* memory_info(PyObject* self, PyObject* args);
PyObject* map_segment(PyObject* self, PyObject* args);
PyObject* map_segment_n(PyObject* self, PyObject* args);

PyObject* get_refsfrom(PyObject* self, PyObject* args);
PyObject* get_refsfromtype(PyObject* self, PyObject* args);
PyObject* get_refsto(PyObject* self, PyObject* args);
PyObject* get_refstotype(PyObject* self, PyObject* args);
PyObject* get_address(PyObject* self, PyObject* args);
PyObject* get_name(PyObject* self, PyObject* args);

PyObject* set_type(PyObject* self, PyObject* args);
PyObject* set_type_ex(PyObject* self, PyObject* args);
PyObject* set_name(PyObject* self, PyObject* args);
PyObject* set_name_ex(PyObject* self, PyObject* args);
PyObject* set_function(PyObject* self, PyObject* args);
PyObject* set_function_ex(PyObject* self, PyObject* args);
PyObject* set_entry(PyObject* self, PyObject* args);
PyObject* add_ref(PyObject* self, PyObject* args);
PyObject* add_problem(PyObject* self, PyObject* args);

PyObject* get_bool(PyObject* self, PyObject* args);
PyObject* get_char(PyObject* self, PyObject* args);
PyObject* get_u8(PyObject* self, PyObject* args);
PyObject* get_u16(PyObject* self, PyObject* args);
PyObject* get_u32(PyObject* self, PyObject* args);
PyObject* get_u64(PyObject* self, PyObject* args);
PyObject* get_i8(PyObject* self, PyObject* args);
PyObject* get_i16(PyObject* self, PyObject* args);
PyObject* get_i32(PyObject* self, PyObject* args);
PyObject* get_i64(PyObject* self, PyObject* args);
PyObject* get_u16be(PyObject* self, PyObject* args);
PyObject* get_u32be(PyObject* self, PyObject* args);
PyObject* get_u64be(PyObject* self, PyObject* args);
PyObject* get_i16be(PyObject* self, PyObject* args);
PyObject* get_i32be(PyObject* self, PyObject* args);
PyObject* get_i64be(PyObject* self, PyObject* args);
PyObject* get_strz(PyObject* self, PyObject* args);
PyObject* get_str(PyObject* self, PyObject* args);
PyObject* get_type(PyObject* self, PyObject* args);

} // namespace redasm::api::python
