#include "psxexe.h"
#include <string_view>

namespace psxexe {

namespace {

// constexpr int PSXEXE_SIGNATURE_SIZE = 8;
constexpr std::string_view PSXEXE_SIGNATURE = "PS-X EXE";
constexpr u32 PSXEXE_TEXT_OFFSET = 0x00000800;
constexpr u32 PSX_USERRAM_START = 0x80000000;
constexpr u32 PSX_USERRAM_END = 0x80200000;

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

    RDBuffer* file = rdbuffer_getfile();
    RDValue* psxheader = rdvalue_create();
    const char* id = nullptr;

    if(!rdbuffer_collecttype(file, 0, "PSX_EXE_HEADER", psxheader) ||
       !rdvalue_getstr(psxheader, "id", &id) || id != PSXEXE_SIGNATURE)
        return false;

    rd_map(PSX_USERRAM_START, PSX_USERRAM_END);

    u32 taddr{}, tsize{}, pc0;
    rdvalue_getu32(psxheader, "t_addr", &taddr);
    rdvalue_getu32(psxheader, "t_size", &tsize);
    rdvalue_getu32(psxheader, "pc0", &pc0);

    rd_mapsegment_n("TEXT", taddr, tsize, PSXEXE_TEXT_OFFSET, tsize,
                    SEG_HASCODE | SEG_HASDATA);

    rd_setentry(pc0, "PSXEXE_EntryPoint");
    return true;
}

} // namespace psxexe
