#include "x86_lifter.h"

namespace x86_lifter {

namespace {

bool needs_calcaddress(const X86Instruction& instr) {
    switch(instr.d.meta.category) {
        case ZYDIS_CATEGORY_CALL:
        case ZYDIS_CATEGORY_UNCOND_BR:
        case ZYDIS_CATEGORY_COND_BR: return true;
        default: break;
    }

    return false;
}

const RDILExpr* lift_op(const X86Instruction& instr, usize idx,
                              RDILPool* pool) {
    const ZydisDecodedOperand& op = instr.ops[idx];
    const RDILExpr* e = nullptr;

    switch(op.type) {
        case ZYDIS_OPERAND_TYPE_REGISTER:
            e = rdil_reg(pool, ZydisRegisterGetString(op.reg.value));
            break;

        case ZYDIS_OPERAND_TYPE_IMMEDIATE: {
            u64 c = op.imm.value.u;

            if(x86_lifter::needs_calcaddress(instr)) {
                auto addr = x86_common::calc_address(instr, idx, instr.address);
                if(addr)
                    c = static_cast<u64>(*addr);
            }

            e = rdil_cnst(pool, c);
            break;
        }

        case ZYDIS_OPERAND_TYPE_MEMORY: {
            const RDILExpr *base = nullptr, *index = nullptr,
                                 *scale = nullptr, *disp = nullptr;
            auto addr = x86_common::calc_address(instr, idx, instr.address);

            if(!addr) {
                if(op.mem.base != ZYDIS_REGISTER_NONE)
                    base = rdil_reg(pool, ZydisRegisterGetString(op.mem.base));
                if(op.mem.index != ZYDIS_REGISTER_NONE) {
                    index =
                        rdil_reg(pool, ZydisRegisterGetString(op.mem.index));
                }
                if(op.mem.scale > 1)
                    scale = rdil_cnst(pool, op.mem.scale);
                if(op.mem.disp.has_displacement)
                    disp = rdil_cnst(pool, op.mem.disp.value);
            }
            else
                disp = rdil_cnst(pool, *addr);

            const RDILExpr* lhs = nullptr;
            const RDILExpr* indexscale =
                (scale && index) ? rdil_mul(pool, index, scale) : index;

            if(base && indexscale)
                lhs = rdil_add(pool, base, indexscale);
            else if(base)
                lhs = base;
            else if(indexscale)
                lhs = indexscale;

            const RDILExpr* m = nullptr;

            if(disp) {
                if(lhs)
                    m = rdil_add(pool, lhs, disp);
                else
                    m = disp;
            }
            else if(lhs)
                m = lhs;
            else
                m = rdil_unknown(pool);

            if((instr.d.mnemonic == ZYDIS_MNEMONIC_LEA) && idx)
                e = m;
            else
                e = rdil_mem(pool, m);

            break;
        }

        default: e = rdil_unknown(pool); break;
    }

    return e;
}

} // namespace

bool lift(const X86Instruction& instr, RDILList* l) {
    RDILPool* pool = rdillist_getpool(l);

    switch(instr.d.mnemonic) {
        case ZYDIS_MNEMONIC_CALL: {
            const RDILExpr* op = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* c = rdil_call(pool, op);
            rdillist_append(l, c);
            break;
        }

        case ZYDIS_MNEMONIC_JMP: {
            const RDILExpr* op = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* c = rdil_goto(pool, op);
            rdillist_append(l, c);
            break;
        }

        default: return false;
    }

    return true;
}

} // namespace x86_lifter
