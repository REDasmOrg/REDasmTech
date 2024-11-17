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
                auto addr = x86_common::calc_address(instr, idx);
                if(addr)
                    c = static_cast<u64>(*addr);

                e = rdil_var(pool, c);
            }
            else if(rd_isaddress(c))
                e = rdil_var(pool, c);
            else
                e = rdil_cnst(pool, c);
            break;
        }

        case ZYDIS_OPERAND_TYPE_MEMORY: {
            const RDILExpr *base = nullptr, *index = nullptr, *scale = nullptr,
                           *disp = nullptr;
            auto addr = x86_common::calc_address(instr, idx);

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
                disp = rdil_var(pool, *addr);

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

const RDILExpr* lift_jump(const X86Instruction& instr, RDILPool* pool) {
    const RDILExpr* cond = nullptr;

    switch(instr.d.mnemonic) {
        case ZYDIS_MNEMONIC_JZ:
            cond = rdil_eq(pool, rdil_sym(pool, "z"), rdil_cnst(pool, 0));
            break;

        case ZYDIS_MNEMONIC_JNZ:
            cond = rdil_ne(pool, rdil_sym(pool, "z"), rdil_cnst(pool, 0));
            break;

        default: return rdil_unknown(pool);
    }

    auto addr = x86_common::calc_address(instr, 0);
    const RDILExpr* t = rdil_var(pool, addr.value_or(instr.ops[0].imm.value.u));
    const RDILExpr* f = rdil_var(pool, instr.address + instr.d.length);
    return rdil_if(pool, cond, rdil_goto(pool, t), rdil_goto(pool, f));
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

        case ZYDIS_MNEMONIC_MOV:
        case ZYDIS_MNEMONIC_LEA: {
            const RDILExpr* dst = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* src = x86_lifter::lift_op(instr, 1, pool);
            const RDILExpr* c = rdil_copy(pool, dst, src);
            rdillist_append(l, c);
            break;
        }

        case ZYDIS_MNEMONIC_PUSH: {
            const RDILExpr* op = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* c = rdil_push(pool, op);
            rdillist_append(l, c);
            break;
        }

        case ZYDIS_MNEMONIC_POP: {
            const RDILExpr* op = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* c = rdil_pop(pool, op);
            rdillist_append(l, c);
            break;
        }

        case ZYDIS_MNEMONIC_CMP: {
            const RDILExpr* left = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* right = x86_lifter::lift_op(instr, 1, pool);
            const RDILExpr* v = rdil_sym(pool, "z");
            const RDILExpr* s = rdil_sub(pool, left, right);
            rdillist_append(l, rdil_copy(pool, v, s));
            break;
        }

        case ZYDIS_MNEMONIC_ENTER: {
            const char* bpreg = ZydisRegisterGetString(x86_common::get_bp());
            const char* spreg = ZydisRegisterGetString(x86_common::get_sp());
            rdillist_append(l, rdil_push(pool, rdil_reg(pool, bpreg)));
            rdillist_append(l, rdil_copy(pool, rdil_reg(pool, bpreg),
                                         rdil_reg(pool, spreg)));
            break;
        }

        case ZYDIS_MNEMONIC_LEAVE: {
            const RDILExpr* sp =
                rdil_reg(pool, ZydisRegisterGetString(x86_common::get_sp()));
            const RDILExpr* bp =
                rdil_reg(pool, ZydisRegisterGetString(x86_common::get_bp()));
            rdillist_append(l, rdil_copy(pool, sp, bp));

            bp = rdil_reg(pool, ZydisRegisterGetString(x86_common::get_bp()));
            rdillist_append(l, rdil_pop(pool, bp));
            break;
        }

        case ZYDIS_MNEMONIC_RET: {
            rdillist_append(l, rdil_pop(pool, rdil_sym(pool, "result")));

            if(instr.d.operand_count) {
                const RDILExpr* sp = rdil_reg(
                    pool, ZydisRegisterGetString(x86_common::get_sp()));

                rdillist_append(
                    l, rdil_add(pool, sp,
                                rdil_cnst(pool, instr.ops[0].imm.value.u)));
            }

            rdillist_append(l, rdil_ret(pool, rdil_sym(pool, "result")));
            break;
        }

        case ZYDIS_MNEMONIC_ADD: {
            const RDILExpr* op1 = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* op2 = x86_lifter::lift_op(instr, 1, pool);
            rdillist_append(l, rdil_copy(pool, op1, rdil_add(pool, op1, op2)));
            break;
        }

        case ZYDIS_MNEMONIC_SUB: {
            const RDILExpr* op1 = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* op2 = x86_lifter::lift_op(instr, 1, pool);
            rdillist_append(l, rdil_copy(pool, op1, rdil_sub(pool, op1, op2)));
            break;
        }

        case ZYDIS_MNEMONIC_MUL: {
            const RDILExpr* op1 = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* op2 = x86_lifter::lift_op(instr, 1, pool);
            rdillist_append(l, rdil_copy(pool, op1, rdil_mul(pool, op1, op2)));
            break;
        }

        case ZYDIS_MNEMONIC_DIV: {
            const RDILExpr* op1 = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* op2 = x86_lifter::lift_op(instr, 1, pool);
            rdillist_append(l, rdil_copy(pool, op1, rdil_div(pool, op1, op2)));
            break;
        }

        case ZYDIS_MNEMONIC_AND: {
            const RDILExpr* op1 = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* op2 = x86_lifter::lift_op(instr, 1, pool);
            rdillist_append(l, rdil_copy(pool, op1, rdil_and(pool, op1, op2)));
            break;
        }

        case ZYDIS_MNEMONIC_OR: {
            const RDILExpr* op1 = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* op2 = x86_lifter::lift_op(instr, 1, pool);
            rdillist_append(l, rdil_copy(pool, op1, rdil_or(pool, op1, op2)));
            break;
        }

        case ZYDIS_MNEMONIC_XOR: {
            const RDILExpr* op1 = x86_lifter::lift_op(instr, 0, pool);
            const RDILExpr* op2 = x86_lifter::lift_op(instr, 1, pool);
            rdillist_append(l, rdil_copy(pool, op1, rdil_xor(pool, op1, op2)));
            break;
        }

        case ZYDIS_MNEMONIC_JZ:
        case ZYDIS_MNEMONIC_JNZ:
            rdillist_append(l, x86_lifter::lift_jump(instr, pool));
            break;

        case ZYDIS_MNEMONIC_NOP: rdillist_append(l, rdil_nop(pool)); break;
        default: return false;
    }

    return true;
}

} // namespace x86_lifter
