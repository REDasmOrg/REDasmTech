#include "elf_dwarf.h"

namespace elf_dwarf {

namespace {

RDAddress do_calc_address(RDAddress base, const RDValue& val, u8 dwarftype) {
    switch(dwarftype & 0x0F) {
        case DW_EH_PE_UDATA2: return base + val.u16_v;
        case DW_EH_PE_UDATA4: return base + val.u32_v;
        case DW_EH_PE_UDATA8: return base + val.u64_v;
        case DW_EH_PE_SDATA2: return base + val.i16_v;
        case DW_EH_PE_SDATA4: return base + val.i32_v;
        case DW_EH_PE_SDATA8: return base + val.i64_v;
        default: break;
    }

    return 0;
}

} // namespace

const char* get_type(u8 dwarftype) {
    switch(dwarftype & 0x0F) {
        case DW_EH_PE_UDATA2: return "u16";
        case DW_EH_PE_UDATA4: return "u32";
        case DW_EH_PE_UDATA8: return "u64";
        case DW_EH_PE_SDATA2: return "i16";
        case DW_EH_PE_SDATA4: return "i32";
        case DW_EH_PE_SDATA8: return "i64";
        default: break;
    }

    return nullptr;
}

usize apply_type(RDAddress addr, u8 dwarftype, RDValue* v) {
    const char* t = elf_dwarf::get_type(dwarftype);
    if(t && rd_settypename(addr, t, v)) return rd_nsizeof(t);
    return 0;
}

RDAddress calc_address(RDAddress pcrel, RDAddress datarel, const RDValue& val,
                       u8 tableenc) {
    u8 fmt = tableenc & 0x0F;
    u8 app = tableenc & 0xF0;

    switch(app) {
        case DW_EH_PE_DATAREL: return do_calc_address(datarel, val, fmt);
        case DW_EH_PE_PCREL: return do_calc_address(pcrel, val, fmt);
        case DW_EH_PE_ABSPTR: return do_calc_address(0, val, fmt);
        default: break;
    }

    return 0;
}

} // namespace elf_dwarf
