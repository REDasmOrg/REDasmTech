#include "psxexe.h"
#include <string_view>

namespace psxexe {

namespace {

// constexpr int PSXEXE_SIGNATURE_SIZE = 8;
constexpr std::string_view PSXEXE_SIGNATURE = "PS-X EXE";
constexpr u32 PSXEXE_TEXT_OFFSET = 0x00000800;
constexpr u32 PSX_USER_RAM_START = 0x80000000;
constexpr u32 PSX_USER_RAM_END = 0x80200000;

const RDStructField PSX_EXE_HEADER[] = {
    {"char[8]", "id"},  {"u32", "text"},    {"u32", "data"},
    {"u32", "pc0"},     {"u32", "gp0"},     {"u32", "t_addr"},
    {"u32", "t_size"},  {"u32", "d_addr"},  {"u32", "d_size"},
    {"u32", "b_addr"},  {"u32", "b_size"},  {"u32", "s_addr"},
    {"u32", "s_size"},  {"u32", "SavedSP"}, {"u32", "SavedFP"},
    {"u32", "SavedGP"}, {"u32", "SavedRA"}, {"u32", "SavedS0"},
    {nullptr, nullptr},
};

} // namespace

bool init(RDLoader*) {
    rd_createstruct("PSX_EXE_HEADER", PSX_EXE_HEADER);
    rd_setbits(32);

    return true;
}

} // namespace psxexe
