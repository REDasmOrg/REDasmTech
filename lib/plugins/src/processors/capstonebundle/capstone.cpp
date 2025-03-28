#include "capstone.h"

Capstone::Capstone(cs_arch arch, cs_mode mode) {
    cs_err err = cs_open(arch, mode, &this->handle);

    if(err) {
        rd_log(cs_strerror(err));
        return;
    }

    cs_option(this->handle, CS_OPT_DETAIL, CS_OPT_ON);
    this->insn = cs_malloc(this->handle);
}

Capstone::~Capstone() {
    if(!this->handle) return;
    cs_free(this->insn, 1);
    cs_close(&this->handle);
}

const char* Capstone::get_mnemonic(u32 id) const {
    return cs_insn_name(this->handle, id);
}

const char* Capstone::get_registername(int regid) const {
    return cs_reg_name(this->handle, regid);
}

RDAddress Capstone::normalize_address(RDAddress address) const {
    return address;
}

bool Capstone::disasm(RDInstruction* instr, u8* data, usize size) const {
    usize n = rd_read(instr->address, data, size);
    if(n != size) return false;

    const auto* pdata = reinterpret_cast<const u8*>(data);
    u64 outaddress = instr->address;
    size_t outsize = size;

    return cs_disasm_iter(this->handle, &pdata, &outsize, &outaddress,
                          this->insn);
}
