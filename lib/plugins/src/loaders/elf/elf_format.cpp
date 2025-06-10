#include "elf_format.h"
#include <cstring>
#include <optional>

// https://www.corsix.org/content/elf-eh-frame

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

void apply_field(RDAddress& a, const char* t, const char* n,
                 RDValue* v = nullptr) {
    rd_settypename(a, t, v);
    rd_setname_ex(a, n, SN_ADDRESS);
    a += rd_nsizeof(t);
};

void apply_str_field(RDAddress& a, const char* t, const char* n, RDValue* v) {
    rd_settypename(a, t, v);
    rd_setname_ex(a, n, SN_ADDRESS);
    a += str_length(&v->str) + 1;
};

void apply_leb_field(RDAddress& a, const char* t, const char* n) {
    RDValue v;
    rd_settypename(a, t, &v);
    rd_setname_ex(a, n, SN_ADDRESS);
    a += v.leb.size;
    rdvalue_destroy(&v);
};

} // namespace

template<int Bits>
void ElfFormat<Bits>::apply_type(const char* tname, const SHDR& shdr) const {
    const usize S = rd_nsizeof(tname);
    const usize N = shdr.sh_size / S;

    RDType t;
    if(rd_createtype_n(tname, N, &t)) rd_settype(shdr.sh_addr, &t, nullptr);
}

template<int Bits>
std::string ElfFormat<Bits>::get_str(int idx, usize shidx,
                                     const std::string& d) const {
    RDBuffer* file = rd_getfile();
    const char* p = nullptr;

    if(rdbuffer_getstrz(file, this->shdr.at(shidx).sh_offset + idx, &p))
        return p;
    return d;
}

template<int Bits>
std::string_view ElfFormat<Bits>::get_strv(int idx, usize shidx) const {
    RDBuffer* file = rd_getfile();
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

    if(n == ".interp")
        rd_settypename(shdr.sh_addr, "str", nullptr);
    else if(n == ".eh_frame_hdr")
        this->process_eh_frame_hdr(shdr);
    else if(n == ".eh_frame")
        this->process_eh_frame(shdr);
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
elf_dwarf::CIEInfo ElfFormat<Bits>::parse_dwarf_cie(RDAddress& addr,
                                                    RDAddress prevaddr,
                                                    u32 len) const {
    RDValue version;
    apply_field(addr, "u8", "version", &version);

    RDValue augstr;
    apply_str_field(addr, "str", "aug_string", &augstr);

    apply_leb_field(addr, "uleb128", "code_align");
    apply_leb_field(addr, "leb128", "data_align");

    if(version.u8_v == 1)
        apply_field(addr, "u8", "return_address_ordinal");
    else
        apply_leb_field(addr, "uleb128", "return_address_ordinal");

    // aug_operands part
    bool has_eh = str_startswith(&augstr.str, "eh");
    usize idx = has_eh ? 2 : 0;
    bool has_z =
        str_length(&augstr.str) > idx && str_at(&augstr.str, idx) == 'z';

    if(has_eh) apply_field(addr, "u32", "eh_ptr"); // NOTE: Pointer
    if(has_z) apply_leb_field(addr, "uleb128", "length");

    RDValue fde_ptr_encoding;
    rdvalue_init(&fde_ptr_encoding);

    if(str_contains(&augstr.str, "R"))
        apply_field(addr, "u8", "fde_ptr_encoding", &fde_ptr_encoding);

    usize cfa_bytecode_len = (len + sizeof(u32)) - (addr - prevaddr);

    RDType cfa_bytecode_type;
    rd_createtype_n("u8", cfa_bytecode_len, &cfa_bytecode_type);
    rd_settype(addr, &cfa_bytecode_type, nullptr);
    rd_setname_ex(addr, "dw_cfa_bytecode", SN_ADDRESS);
    addr += cfa_bytecode_len;

    elf_dwarf::CIEInfo cieinfo = {
        .aug_string = augstr.str.data,
        .fde_ptr_encoding = fde_ptr_encoding.u8_v,
    };

    rdvalue_destroy(&fde_ptr_encoding);
    rdvalue_destroy(&augstr);
    rdvalue_destroy(&version);
    return cieinfo;
}

template<int Bits>
void ElfFormat<Bits>::parse_dwarf_fde(RDAddress& addr, RDAddress prevaddr,
                                      u32 len,
                                      const elf_dwarf::CIEInfo& cie) const {
    RDValue funcstart;
    usize sz = elf_dwarf::apply_type(addr, cie.fde_ptr_encoding, &funcstart);
    rd_setname_ex(addr, "func_start", SN_ADDRESS);
    addr += sz;

    sz = elf_dwarf::apply_type(addr, cie.fde_ptr_encoding, nullptr);
    rd_setname_ex(addr, "func_length", SN_ADDRESS);
    addr += sz;

    if(cie.aug_string.find('z') != std::string::npos)
        apply_leb_field(addr, "uleb128", "length");

    usize cfa_bytecode_len = (len + sizeof(u32)) - (addr - prevaddr);

    RDType cfa_bytecode_type;
    rd_createtype_n("u8", cfa_bytecode_len, &cfa_bytecode_type);
    rd_settype(addr, &cfa_bytecode_type, nullptr);
    rd_setname_ex(addr, "dw_cfa_bytecode", SN_ADDRESS);
    addr += cfa_bytecode_len;

    rdvalue_destroy(&funcstart);
}

template<int Bits>
void ElfFormat<Bits>::process_init_fini(const SHDR& shdr,
                                        const std::string& prefix) const {
    constexpr const char* TYPE = Bits == 64 ? "u64" : "u32";
    const usize S = rd_nsizeof(TYPE);
    const usize N = shdr.sh_size / S;

    RDValue v;
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
        rdvalue_destroy(&v);
    }
}

template<int Bits>
void ElfFormat<Bits>::process_strings(const SHDR& shdr) const {
    RDAddress address = shdr.sh_addr;
    RDAddress endaddress = address++ + shdr.sh_size; // First is always 00
    RDValue v;

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
void ElfFormat<Bits>::process_eh_frame_hdr(const SHDR& shdr) const {
    if(!shdr.sh_addr || !shdr.sh_size) return;
    rd_settypename(shdr.sh_addr, "ELF_EH_FRAME_HDR", nullptr);

    ElfEhFrameHdr framehdr;
    rd_read(shdr.sh_addr, &framehdr, sizeof(ElfEhFrameHdr));
    if(framehdr.version != 1) return;

    RDAddress addr = shdr.sh_addr + sizeof(ElfEhFrameHdr);

    // eh_frame_ptr
    usize n = elf_dwarf::apply_type(addr, framehdr.eh_frame_ptr_enc & 0xF);
    if(!n) return;
    rd_setname(addr, "eh_frame_ptr");
    addr += n;

    // fde_count_enc
    RDValue valc;
    n = elf_dwarf::apply_type(addr, framehdr.fde_count_enc & 0xF, &valc);
    if(!n) return;
    rd_setname(addr, "fde_count");
    addr += n;

    u64 c = 0;
    const char* tabletype = nullptr;
    RDType tableentry_type;
    RDValue tableentry;
    const RDValue* e;
    const RDValueHNode* it;
    RDAddress ep;

    if(!rdvalue_getinteger(&valc, &c)) goto cleanup;

    tabletype = elf_dwarf::get_type(framehdr.table_enc & 0xF);

    if(tabletype && c) {
        std::vector<RDStructFieldDecl> tableentry_decl = {
            {.type = tabletype, .name = "start_pc"},
            {.type = tabletype, .name = "fde_ptr"},
            {.type = nullptr, .name = nullptr},
        };

        rd_createstruct("EH_FRAME_HDR_TABLE_ENTRY", tableentry_decl.data());

        if(rd_createtype_n("EH_FRAME_HDR_TABLE_ENTRY", c, &tableentry_type)) {
            rd_settype(addr, &tableentry_type, &tableentry);
            rd_setname(addr, "eh_frame_hdr_sorted_table");

            slice_foreach(e, &tableentry.list) {
                hmap_get(it, &e->dict, "start_pc", RDValueHNode, hnode,
                         !std::strcmp(it->key, "start_pc"));

                if(!it) continue;

                ep = elf_dwarf::calc_address(addr, shdr.sh_addr, it->value,
                                             framehdr.table_enc);

                if(ep) rd_setfunction(ep);
            }
        }
    }

cleanup:
    rdvalue_destroy(&valc);
    rdvalue_destroy(&tableentry);
}

template<int Bits>
void ElfFormat<Bits>::process_eh_frame(const SHDR& shdr) const {
    RDAddress addr = shdr.sh_addr, prevaddr = shdr.sh_addr;
    RDAddress endaddr = shdr.sh_addr + shdr.sh_size;

    std::optional<elf_dwarf::CIEInfo> cie;

    while(addr < endaddr) {
        RDValue len, id;
        apply_field(addr, "u32", "length", &len);
        if(!len.u32_v) break;

        rd_settypename(addr, "u32", &id);

        if(id.u32_v) {
            ct_assume(cie.has_value());
            rd_setname_ex(addr, "negated_cie_offset", SN_ADDRESS);
            addr += rd_nsizeof("u32");
            this->parse_dwarf_fde(addr, prevaddr, len.u32_v, *cie);
        }
        else { // CIE
            rd_setname_ex(addr, "zero", SN_ADDRESS);
            addr += rd_nsizeof("u32");
            cie = this->parse_dwarf_cie(addr, prevaddr, len.u32_v);
        }

        prevaddr = addr;
    }
}

template<int Bits>
void ElfFormat<Bits>::process_symtab_offset(const SHDR& shdr) const {
    std::vector<SYM> symtab(shdr.sh_size / sizeof(SYM));

    RDBuffer* file = rd_getfile();
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
