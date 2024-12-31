#include "elf_sections.h"

namespace elf_sections {

namespace {

template<typename T>
bool value_getint(int bits, const RDValue* v, T& val) {
    if(bits == 64) {
        u64 temp;
        if(!rdvalue_getu64(v, nullptr, &temp))
            return false;
        val = temp;
    }
    else {
        u32 temp;
        if(!rdvalue_getu32(v, nullptr, &temp))
            return false;
        val = temp;
    }

    return true;
}

void apply_type(const char* tname, RDAddress address, u64 size, RDValue* v) {
    const usize S = rd_sizeof(tname);
    const usize N = size / S;

    RDType t;
    if(rd_createtype_n(tname, N, &t))
        rd_settype(address, &t, v);
}

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

        RDAddress addr;
        if(elf_sections::value_getint(s.bits(), v, addr) || !addr ||
           addr == -1ULL)
            continue;

        std::string fn = prefix + rd_tohex_n(addr, 0);
        rd_setfunction(addr);
        rd_setname(addr, fn.c_str());
    }
}

template<int bits>
void process_symtab(const ElfState& s, RDOffset offset, u64 size, u32 shlink) {
    const int ITEM_SIZE = sizeof(ElfSym<bits>);
    const int N = size / ITEM_SIZE;
    std::vector<ElfSym<bits>> symtab(N);

    if(!rdbuffer_read(s.file, offset, symtab.data(), ITEM_SIZE * N))
        return;

    for(const auto& sym : symtab) {
        if(!sym.st_name || !sym.st_value)
            continue;

        std::string_view nam = elf_state::get_stringv(s, sym.st_name, shlink);
        if(nam.empty())
            continue;

        u8 type = elf_st_type(sym.st_info);
        u8 bind = elf_st_bind(sym.st_info);

        switch(type) {
            case STT_FUNC: {
                if(bind != STB_GLOBAL) {
                    rd_setfunction(sym.st_value);
                    rd_setname(sym.st_value, nam.data());
                }
                else
                    rd_setentry(sym.st_value, nam.data());

                break;
            }

            case STT_OBJECT: {
                RDType t;

                if(sym.st_size && rd_intfrombytes(sym.st_size, false, &t) &&
                   rd_settype(sym.st_value, &t, nullptr)) {
                    rd_setname(sym.st_value, nam.data());
                }

                break;
            }

            default: break;
        }
    }
}

void process_symtab_addr(const ElfState& s, RDAddress address, u64 size,
                         u32 shlink) {
    RDValue* v = rdvalue_create();
    elf_sections::apply_type("ELF_SYM", address, size, v);

    for(usize i = 0; i < rdvalue_getlength(v); i++) {
        const RDValue* item;
        if(!rdvalue_at(v, i, &item))
            break;

        u32 iname;
        if(!rdvalue_getu32(item, "st_name", &iname) || !iname)
            continue;

        // TODO(davide): merge code with .symtab
        // std::string_view sym = elf_state::get_stringv(s, iname, shlink);
        // std::puts((" >>> " + std::string{sym}).c_str());
    }
}

template<int bits>
void process_rel(const ElfState& s, RDAddress address, u64 size, u32 shlink) {
    RDValue* v = rdvalue_create();
    elf_sections::apply_type("ELF_REL", address, size, v);

    for(usize i = 0; i < rdvalue_getlength(v); i++) {
        const RDValue* item;
        if(!rdvalue_at(v, i, &item))
            break;

        const RDValue *roffset = nullptr, *rinfo = nullptr;
        RDAddress iaddr;
        u64 iinfo;

        if(!rdvalue_get(item, "r_offset", &roffset) ||
           !rdvalue_get(item, "r_info", &rinfo) ||
           !elf_sections::value_getint(bits, roffset, iaddr) ||
           !elf_sections::value_getint(bits, rinfo, iinfo))
            continue;

        auto itype = elf_r_type<bits>(iinfo);
        auto sym = elf_r_sym<bits>(iinfo);

        switch(itype) {
            case R_386_JMP_SLOT: {
                // std::string_view n = elf_state::get_stringv(s, sym, shlink);
                // if(!n.empty())
                // rd_setname(iaddr, n.data());
                break;
            }

            default: break;
        }
    }
}

void process_rela(RDAddress address, u64 size, u32 shlink) {
    RDValue* v = rdvalue_create();
    elf_sections::apply_type("ELF_RELA", address, size, v);
}

void process_strings(RDAddress address, u64 size) {
    RDAddress endaddress = address++ + size; // First is always 00
    RDValue* v = rdvalue_create();

    while(address < endaddress) {
        if(!rd_settypename(address, "str", v))
            break;

        address += rdvalue_getlength(v) + 1;
    }
}

} // namespace

void parse_address(const ElfState& s, std::string_view name, u32 shtype,
                   RDAddress address, u64 size, u32 shlink) {
    if(!size)
        return;

    switch(shtype) {
        case SHT_INIT_ARRAY:
            elf_sections::process_init_fini(s, address, size, "init_");
            return;

        case SHT_FINI_ARRAY:
            elf_sections::process_init_fini(s, address, size, "fini_");
            return;

        case SHT_DYNAMIC:
            elf_sections::apply_type("ELF_DYN", address, size, nullptr);
            return;

        case SHT_DYNSYM:
        case SHT_SYMTAB:
            elf_sections::process_symtab_addr(s, address, size, shlink);
            return;

        case SHT_RELA: {
            elf_sections::process_rela(address, size, shlink);
            return;
        }

        case SHT_REL: {
            if(s.bits() == 64)
                elf_sections::process_rel<64>(s, address, size, shlink);
            else
                elf_sections::process_rel<32>(s, address, size, shlink);
            return;
        }

        case SHT_STRTAB: elf_sections::process_strings(address, size); return;
        default: break;
    }

    if(name == ".interp")
        rd_settypename(address, "str", nullptr);
}

void parse_offset(const ElfState& s, u32 shtype, RDOffset offset, u64 size,
                  u32 shlink) {
    if(!size)
        return;

    switch(shtype) {
        case SHT_SYMTAB: {
            if(s.bits() == 64)
                elf_sections::process_symtab<64>(s, offset, size, shlink);
            else
                elf_sections::process_symtab<32>(s, offset, size, shlink);
            return;
        }

        default: break;
    }
}

} // namespace elf_sections
