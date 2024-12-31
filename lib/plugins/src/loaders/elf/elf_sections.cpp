#include "elf_sections.h"

namespace elf_sections {

namespace {

void process_init_fini(const ElfState& s, RDAddress address, u64 size,
                       const std::string& prefix) {
    const char* type = s.bits() == 64 ? "u64" : "u32";
    const usize S = rd_sizeof(type);
    const usize N = size / S;
    RDAddress addr = address;

    RDValue* v = rdvalue_create();

    for(usize i = 0; i < N; i++, addr += S) {
        if(!rd_settypename(addr, type, v))
            continue;

        if(s.bits() == 64) {
            u64 faddr;
            if(!rdvalue_getu64(v, ".", &faddr) || !faddr || faddr == -1ULL)
                continue;
            std::string fn = prefix + rd_tohex_n(faddr, 0);
            rd_setfunction(faddr);
            rd_setname(faddr, fn.c_str());
        }
        else {
            u32 faddr;
            if(!rdvalue_getu32(v, ".", &faddr) || !faddr || faddr == -1U)
                continue;
            std::string fn = prefix + rd_tohex_n(faddr, 0);
            rd_setfunction(faddr);
            rd_setname(faddr, fn.c_str());
        }
    }
}

} // namespace

void parse(const ElfState& s, std::string_view name, RDAddress address,
           u64 size) {
    if(!size)
        return;

    if(name == ".interp")
        rd_settypename(address, "str", nullptr);
    else if(name == ".ctors")
        elf_sections::process_init_fini(s, address, size, "ctors_");
    else if(name == ".dtors")
        elf_sections::process_init_fini(s, address, size, "dtors_");
    else if(name == ".init_array")
        elf_sections::process_init_fini(s, address, size, "init_");
    else if(name == ".fini_array")
        elf_sections::process_init_fini(s, address, size, "fini_");
}

} // namespace elf_sections
