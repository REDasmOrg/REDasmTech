#include "com.h"
#include <string_view>

namespace com {

using namespace std::string_view_literals;

namespace {

constexpr u16 MSEG_SIZE = 0xff00;

bool accept(const RDLoaderPlugin*, const RDLoaderRequest* req) {
    return req->ext == "COM"sv && rdbuffer_getlength(req->file) <= MSEG_SIZE;
}

bool load(RDLoader*) {
    RDBuffer* file = rdbuffer_getfile();
    usize len = rdbuffer_getlength(file);

    rd_map_n(0, 0x10000);
    rd_setprocessor("x86_16_real");
    rd_mapsegment_n("MEM", 0x100, MSEG_SIZE, 0, len, SEG_HASCODE | SEG_HASDATA);
    rd_setentry(0x100, "_COM_EntryPoint_");
    return true;
}

} // namespace

RDLoaderPlugin loader = {
    .level = REDASM_API_LEVEL,
    .id = "com",
    .name = "COM Executable",
    .accept = com::accept,
    .load = com::load,
};

} // namespace com
