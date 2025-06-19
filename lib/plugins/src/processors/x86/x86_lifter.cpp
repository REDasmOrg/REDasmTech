#include "x86_lifter.h"
#include "x86_common.h"

namespace x86_lifter {

namespace {

const RDILExpr* lift_op(const RDInstruction* instr, usize idx, RDILPool* pool) {
    const RDOperand& op = instr->operands[idx];
    const RDILExpr* e = nullptr;

    switch(op.type) {
        case OP_REG: e = rdil_reg(pool, op.reg); break;
        case OP_ADDR: e = rdil_var(pool, op.addr); break;
        case OP_IMM: e = rdil_cnst(pool, op.imm); break;

        case OP_MEM: {
            if(instr->id == ZYDIS_MNEMONIC_LEA)
                e = rdil_var(pool, op.mem);
            else
                e = rdil_mem(pool, rdil_var(pool, op.mem));
            break;
        }

        case OP_PHRASE: {
            const RDILExpr* base = rdil_reg(pool, op.phrase.base);
            const RDILExpr* index = rdil_reg(pool, op.phrase.index);
            e = rdil_add(pool, base, index);
            break;
        }

        case OP_DISPL: {
            const RDILExpr *base = nullptr, *index = nullptr, *scale = nullptr,
                           *disp = nullptr;

            if(op.displ.base != ZYDIS_REGISTER_NONE)
                base = rdil_reg(pool, op.displ.base);

            if(op.displ.index != ZYDIS_REGISTER_NONE)
                index = rdil_reg(pool, op.displ.index);

            if(op.displ.scale > 1) scale = rdil_cnst(pool, op.displ.scale);

            if(op.displ.displ) disp = rdil_cnst(pool, op.displ.displ);

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

            if((instr->id == ZYDIS_MNEMONIC_LEA) && idx)
                e = m;
            else
                e = rdil_mem(pool, m);

            break;
        }

        default: e = rdil_unknown(pool); break;
    }

    return e;
}

const RDILExpr* lift_jump(const RDInstruction* instr, RDILPool* pool) {
    const RDILExpr* cond = nullptr;

    switch(instr->id) {
        case ZYDIS_MNEMONIC_JZ:
            cond = rdil_eq(pool, rdil_sym(pool, "z"), rdil_cnst(pool, 0));
            break;

        case ZYDIS_MNEMONIC_JNZ:
            cond = rdil_ne(pool, rdil_sym(pool, "z"), rdil_cnst(pool, 0));
            break;

        default: return rdil_unknown(pool);
    }

    const RDILExpr* t = x86_lifter::lift_op(instr, 0, pool);
    const RDILExpr* f = rdil_var(pool, instr->address + instr->length);
    return rdil_if(pool, cond, rdil_goto(pool, t), rdil_goto(pool, f));
}

} // namespace

bool lift(RDProcessor*, RDILList* l, const RDInstruction* instr) {
    RDILPool* pool = rdillist_getpool(l);

    switch(instr->id) {
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
            rdillist_append(
                l, rdil_push(pool, rdil_reg(pool, x86_common::get_bp())));
            rdillist_append(l, rdil_copy(pool,
                                         rdil_reg(pool, x86_common::get_bp()),
                                         rdil_reg(pool, x86_common::get_sp())));
            break;
        }

        case ZYDIS_MNEMONIC_LEAVE: {
            const RDILExpr* sp = rdil_reg(pool, x86_common::get_sp());
            const RDILExpr* bp = rdil_reg(pool, x86_common::get_bp());
            rdillist_append(l, rdil_copy(pool, sp, bp));

            bp = rdil_reg(pool, x86_common::get_bp());
            rdillist_append(l, rdil_pop(pool, bp));
            break;
        }

        case ZYDIS_MNEMONIC_RET: {
            rdillist_append(l, rdil_pop(pool, rdil_sym(pool, "result")));

            if(instr->operands[0].type == OP_IMM) {
                const RDILExpr* sp = rdil_reg(pool, x86_common::get_sp());

                rdillist_append(
                    l, rdil_add(pool, sp,
                                rdil_cnst(pool, instr->operands[0].imm)));
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
