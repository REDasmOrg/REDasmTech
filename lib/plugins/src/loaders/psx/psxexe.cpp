#include "psxexe.h"
#include <string_view>

namespace psxexe {

namespace {

constexpr int PSXEXE_SIGNATURE_SIZE = 8;
constexpr std::string_view PSXEXE_SIGNATURE = "PS-X EXE";
constexpr u32 PSXEXE_TEXT_OFFSET = 0x00000800;
constexpr u32 PSX_USERRAM_START = 0x80000000;
constexpr u32 PSX_USERRAM_END = 0x80200000;

#pragma pack(push, 1)
struct PsxExeHeader {
    char id[PSXEXE_SIGNATURE_SIZE];
    u32 text, data;
    u32 pc0, gp0;
    u32 t_addr, t_size;
    u32 d_addr, d_size;
    u32 b_addr, b_size;
    u32 s_addr, s_size;
    u32 SavedSP, SavedFP, SavedGP, SavedRA, SavedS0;
};
#pragma pack(pop)

bool accept(const RDLoaderPlugin*, const RDLoaderRequest* req) {
    PsxExeHeader psxheader;
    usize n = rdbuffer_read(req->file, 0, &psxheader, sizeof(PsxExeHeader));
    return n == sizeof(PsxExeHeader) &&
           !PSXEXE_SIGNATURE.compare(0, PSXEXE_SIGNATURE_SIZE, psxheader.id,
                                     PSXEXE_SIGNATURE_SIZE);
}

bool load(RDLoader*) {
    PsxExeHeader psxheader;
    rdbuffer_read(rdbuffer_getfile(), 0, &psxheader, sizeof(PsxExeHeader));
    rd_setprocessor("mips32le");
    rd_map(PSX_USERRAM_START, PSX_USERRAM_END);

    rd_mapsegment_n("TEXT", psxheader.t_addr, psxheader.t_size,
                    PSXEXE_TEXT_OFFSET, psxheader.t_size,
                    SEG_HASCODE | SEG_HASDATA);

    rd_setentry(psxheader.pc0, "PSXEXE_EntryPoint");
    return true;
}

} // namespace

RDLoaderPlugin loader = {
    .level = REDASM_API_LEVEL,
    .id = "psxexe",
    .name = "PS-X Executable",
    .accept = psxexe::accept,
    .load = psxexe::load,
};

} // namespace psxexe
