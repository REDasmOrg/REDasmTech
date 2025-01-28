#include "com.h"
#include <string_view>

namespace com {

using namespace std::string_view_literals;

namespace {

constexpr u16 COM_ENTRY = 0x0100;
constexpr u16 MSEG_SIZE = 0xff00;

bool accept(const RDLoaderPlugin*, const RDLoaderRequest* req) {
    return req->ext == "COM"sv && rdbuffer_getlength(req->file) <= MSEG_SIZE;
}

bool load(RDLoader*, RDBuffer* file) {
    usize len = rdbuffer_getlength(file);

    rd_map_n(0, 0x10000);
    rd_mapsegment_n("MEM", COM_ENTRY, MSEG_SIZE, 0, len, SP_RWX);
    rd_setentry(COM_ENTRY, "_COM_EntryPoint_");
    return true;
}

} // namespace

RDLoaderPlugin loader = {
    .level = REDASM_API_LEVEL,
    .id = "com",
    .name = "COM Executable",
    .accept = com::accept,
    .load = com::load,
    .get_processor = [](RDLoader*) { return "x86_16_real"; },
};

} // namespace com
