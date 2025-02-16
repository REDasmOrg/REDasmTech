#include "com.h"
#include <string_view>

namespace com {

using namespace std::string_view_literals;

namespace {

constexpr u16 COM_ENTRY = 0x0100;
constexpr u16 MSEG_SIZE = 0xffff;

bool parse(RDLoader*, const RDLoaderRequest* req) {
    return req->ext == "COM"sv && rdbuffer_getlength(req->file) <= 0xff00;
}

bool load(RDLoader*, RDBuffer* file) {
    usize len = rdbuffer_getlength(file);
    rd_addsegment_n("MEM", 0, MSEG_SIZE, SP_RWX, 16);
    rd_mapfile_n(0, COM_ENTRY, len);
    rd_setentry(COM_ENTRY, "_COM_EntryPoint_");
    return true;
}

} // namespace

RDLoaderPlugin loader = {
    .level = REDASM_API_LEVEL,
    .id = "com",
    .name = "COM Executable",
    .parse = com::parse,
    .load = com::load,
    .get_processor = [](RDLoader*) { return "x86_16_real"; },
};

} // namespace com
