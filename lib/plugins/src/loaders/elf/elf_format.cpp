#include "elf_format.h"
#include <optional>

namespace {

template<int Bits>
std::optional<RDAddress> read_address(RDAddress address) {
    if constexpr(Bits == 64) {
        u64 v;
        bool ok = rd_getu64(address, &v);
        if(ok) return v;
    }
    else {
        u32 v;
        bool ok = rd_getu32(address, &v);
        if(ok) return v;
    }

    return std::nullopt;
}

} // namespace

template<int Bits>
void ElfFormat<Bits>::apply_type(const char* tname, const SHDR& shdr) const {
    const usize S = rd_nsizeof(tname);
    const usize N = shdr.sh_size / S;

    RDType t;
    if(rdtype_create_n(tname, N, &t)) rd_settype(shdr.sh_addr, &t, nullptr);
}

template<int Bits>
usize ElfFormat<Bits>::map_memory() const {
    RDAddress start = -1ULL, end = 0;
    usize phdridx = this->phdr.size();

    for(usize i = 0; i < this->phdr.size(); i++) {
        const auto& p = this->phdr[i];

        switch(p.p_type) {
            case PT_LOAD: {
                start = std::min<RDAddress>(start, p.p_vaddr);

                u64 asize = p.p_memsz; // Align size
                if(usize diff = asize % p.p_align; diff)
                    asize += (p.p_align - diff);

                RDAddress pend = p.p_vaddr + asize;
                end = std::max<RDAddress>(end, pend);
                break;
            }

            case PT_PHDR: phdridx = i; break;
            default: break;
        }
    }

    rd_map(start, end);
    return phdridx;
}

template<int Bits>
std::string ElfFormat<Bits>::get_str(int idx, usize shidx,
                                     const std::string& d) const {
    RDBuffer* file = rdbuffer_getfile();
    const char* p = nullptr;

    if(rdbuffer_getstrz(file, this->shdr.at(shidx).sh_offset + idx, &p))
        return p;
    return d;
}

template<int Bits>
std::string_view ElfFormat<Bits>::get_strv(int idx, usize shidx) const {
    RDBuffer* file = rdbuffer_getfile();
    const char* p = nullptr;

    if(rdbuffer_getstrz(file, this->shdr.at(shidx).sh_offset + idx, &p))
        return p;
    return {};
}

template<int Bits>
void ElfFormat<Bits>::parse_section_address(usize i) const {
    const SHDR& shdr = this->shdr.at(i);
    if(!shdr.sh_size) return;

    switch(shdr.sh_type) {
        case SHT_DYNSYM:
        case SHT_SYMTAB: this->process_symtab_address(shdr); return;

        case SHT_DYNAMIC: this->apply_type("ELF_DYN", shdr); return;
        case SHT_INIT_ARRAY: this->process_init_fini(shdr, "init_"); return;
        case SHT_FINI_ARRAY: this->process_init_fini(shdr, "fini_"); return;
        case SHT_STRTAB: this->process_strings(shdr); return;
        case SHT_RELA: this->process_rela(shdr); return;
        case SHT_REL: this->process_rel(shdr); return;
        default: break;
    }

    std::string_view n = this->get_strv(shdr.sh_name, this->ehdr.e_shstrndx);
    if(n == ".interp") rd_settypename(shdr.sh_addr, "str", nullptr);
}

template<int Bits>
void ElfFormat<Bits>::parse_section_offset(usize i) const {
    const SHDR& shdr = this->shdr.at(i);
    if(!shdr.sh_size) return;

    switch(shdr.sh_type) {
        case SHT_SYMTAB: {
            this->process_symtab_offset(shdr);
            return;
        }

        default: break;
    }
}

template<int Bits>
void ElfFormat<Bits>::process_init_fini(const SHDR& shdr,
                                        const std::string& prefix) const {
    constexpr const char* TYPE = Bits == 64 ? "u64" : "u32";
    const usize S = rd_nsizeof(TYPE);
    const usize N = shdr.sh_size / S;

    RDValue v{};
    RDAddress addr = shdr.sh_addr;

    for(usize i = 0; i < N; i++, addr += S) {
        if(!rd_settypename(addr, TYPE, &v)) continue;

        RDAddress itemaddr;

        if constexpr(Bits == 64)
            itemaddr = v.u64_v;
        else
            itemaddr = v.u32_v;

        if(!itemaddr || itemaddr == -1ULL) continue;

        std::string fn = prefix + rd_tohex_n(addr, 0);
        rd_setfunction(addr);
        rd_setname(addr, fn.c_str());
    }

    rdvalue_destroy(&v);
}

template<int Bits>
void ElfFormat<Bits>::process_strings(const SHDR& shdr) const {
    RDAddress address = shdr.sh_addr;
    RDAddress endaddress = address++ + shdr.sh_size; // First is always 00
    RDValue v{};

    while(address < endaddress) {
        if(!rd_settypename(address, "str", &v)) break;
        address += rdvalue_getlength(&v) + 1;
        rdvalue_destroy(&v);
    }
}

template<int Bits>
void ElfFormat<Bits>::process_symtab_address(const SHDR& shdr) const {
    this->apply_type("ELF_SYM", shdr);
}

template<int Bits>
void ElfFormat<Bits>::process_rela(const SHDR& shdr) const {
    this->apply_type("ELF_RELA", shdr);
}

template<int Bits>
void ElfFormat<Bits>::process_rel(const SHDR& shdr) const {
    this->apply_type("ELF_REL", shdr);

    if(shdr.sh_link >= this->shdr.size()) return;
    const SHDR& symsect = this->shdr[shdr.sh_link];
    if(symsect.sh_type != SHT_DYNSYM) return;

    std::vector<SYM> symbols(symsect.sh_size / sizeof(SYM));
    if(!rd_read(symsect.sh_addr, symbols.data(), symsect.sh_size)) return;

    std::vector<REL> rels(shdr.sh_size / sizeof(REL));
    if(!rd_read(shdr.sh_addr, rels.data(), shdr.sh_size)) return;

    for(const REL& rel : rels) {
        if(!rel.r_offset) continue;

        auto sym = elf_r_sym<Bits>(rel.r_info);
        if(!sym) continue;

        switch(elf_r_type<Bits>(rel.r_info)) {
            case R_386_JMP_SLOT: {
                auto reladdr = read_address<Bits>(rel.r_offset);

                if(reladdr && sym < symbols.size() && symbols[sym].st_name) {
                    std::string_view sv =
                        this->get_strv(symbols[sym].st_name, symsect.sh_link);
                    if(!sv.empty()) rd_setname(*reladdr, sv.data());
                }

                break;
            }

            default: break;
        }
    }
}

template<int Bits>
void ElfFormat<Bits>::process_symtab_offset(const SHDR& shdr) const {
    std::vector<SYM> symtab(shdr.sh_size / sizeof(SYM));

    RDBuffer* file = rdbuffer_getfile();
    if(!rdbuffer_read(file, shdr.sh_offset, symtab.data(), shdr.sh_size))
        return;

    for(const auto& sym : symtab) {
        if(!sym.st_name || !sym.st_value) continue;

        std::string_view nam = this->get_strv(sym.st_name, shdr.sh_link);
        if(nam.empty()) continue;

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

// Explicit instantation
template struct ElfFormat<32>;
template struct ElfFormat<64>;
