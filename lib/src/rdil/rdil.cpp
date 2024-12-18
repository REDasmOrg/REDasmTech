#include "rdil.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"
#include "../surface/renderer.h"
#include <limits>
#include <unordered_map>

#define RDIL_N(x) {x, #x}

namespace redasm::rdil {

namespace {

enum class WalkType : u8 {
    NORMAL = 0,
    MNEMONIC,
    WHITESPACE,
};

void wrap_format(const ILExpr* e, std::string& res);

bool has_value(const ILExpr* e) {
    switch(e->op) {
        case RDIL_VAR:
        case RDIL_REG:
        case RDIL_SYM:
        case RDIL_CNST: return true;

        default: break;
    }

    return false;
}

std::string text_op(const ILExpr* e) {
    switch(e->op) {
        case RDIL_ADD: return "+";
        case RDIL_SUB: return "-";
        case RDIL_MUL: return "*";
        case RDIL_DIV: return "/";
        case RDIL_MOD: return "%";
        case RDIL_AND: return "&";
        case RDIL_OR: return "|";
        case RDIL_XOR: return "^";
        case RDIL_NOT: return "~";
        case RDIL_LSL: return "<<";
        case RDIL_LSR: return ">>";
        case RDIL_ASL: return "<<<";
        case RDIL_ASR: return ">>>";
        case RDIL_EQ: return "==";
        case RDIL_NE: return "!=";
        case RDIL_LT: return "<";
        case RDIL_LE: return "<=";
        case RDIL_GT: return ">";
        case RDIL_GE: return ">=";
        default: break;
    }

    return "???";
}

template<typename ToStringCallback>
void to_string(const ILExpr* e, ToStringCallback cb);

template<typename ToStringCallback>
void wrap_walk(const ILExpr* e, ToStringCallback cb) {
    if(rdil::has_value(e) || (e->op == RDIL_MEM)) {
        rdil::to_string(e, cb);
        return;
    }

    cb(e, "(", WalkType::NORMAL);
    rdil::to_string(e, cb);
    cb(e, ")", WalkType::NORMAL);
}

bool get_format_impl(const ILExpr* e, std::string& res) {
    switch(e->op) {
        case RDIL_ADD:
        case RDIL_SUB:
        case RDIL_MUL:
        case RDIL_DIV:
        case RDIL_AND:
        case RDIL_OR:
        case RDIL_XOR:
        case RDIL_EQ:
        case RDIL_NE:
        case RDIL_LT:
        case RDIL_LE:
        case RDIL_GT:
        case RDIL_GE:
            rdil::wrap_format(e->l, res);
            res += rdil::text_op(e);
            rdil::wrap_format(e->r, res);
            break;

        case RDIL_NOT:
            res += rdil::text_op(e);
            rdil::wrap_format(e->u, res);
            break;

        case RDIL_IF:
            res += "if(";
            rdil::get_format_impl(e->cond, res);
            res += ")";
            rdil::get_format_impl(e->t, res);
            res += " else ";
            rdil::get_format_impl(e->f, res);
            break;

        case RDIL_COPY:
            rdil::get_format_impl(e->dst, res);
            res += " = ";
            rdil::get_format_impl(e->src, res);
            break;

        case RDIL_CALL:
            rdil::get_format_impl(e->u, res);
            res += "()";
            break;

        case RDIL_INT:
            res += "int ";
            rdil::get_format_impl(e->u, res);
            break;

        case RDIL_GOTO:
            res += "goto ";
            rdil::get_format_impl(e->u, res);
            break;

        case RDIL_MEM:
            res += "[";
            rdil::get_format_impl(e->u, res);
            res += "]";
            break;

        case RDIL_PUSH:
            res += "push(";
            rdil::get_format_impl(e->u, res);
            res += ")";
            break;

        case RDIL_POP:
            rdil::get_format_impl(e->u, res);
            res += " = pop()";
            break;

        case RDIL_RET:
            res += "ret(";
            rdil::get_format_impl(e->cond, res);
            res += ")";
            break;

        case RDIL_UNKNOWN: res += "unknown "; break;
        case RDIL_NOP: res += "nop "; break;
        case RDIL_CNST: res += "cnst"; break;
        case RDIL_VAR: res += "var"; break;
        case RDIL_REG: res += "reg"; break;
        case RDIL_SYM: res += "sym"; break;
        default: except("Unknown IL Expression"); break;
    }

    return true;
}

void wrap_format(const ILExpr* e, std::string& res) {
    if(rdil::has_value(e) || e->op == RDIL_MEM) {
        rdil::get_format_impl(e, res);
        return;
    }

    res += "(";
    rdil::get_format_impl(e, res);
    res += ")";
}

template<typename ToStringCallback>
void to_string(const ILExpr* e, ToStringCallback cb) {
    switch(e->op) {
        case RDIL_ADD:
        case RDIL_SUB:
        case RDIL_MUL:
        case RDIL_DIV:
        case RDIL_MOD:
        case RDIL_AND:
        case RDIL_OR:
        case RDIL_XOR:
        case RDIL_LSL:
        case RDIL_LSR:
        case RDIL_ASL:
        case RDIL_ASR:
        case RDIL_EQ:
        case RDIL_NE:
        case RDIL_LT:
        case RDIL_LE:
        case RDIL_GT:
        case RDIL_GE:
            rdil::wrap_walk(e->l, cb);
            cb(e, rdil::text_op(e), WalkType::NORMAL);
            rdil::to_string(e->r, cb);
            break;

        case RDIL_ROL:
        case RDIL_ROR:
            cb(e, (e->op == RDIL_ROL) ? "rol" : "ror", WalkType::MNEMONIC);
            cb(e, "(", WalkType::NORMAL);
            rdil::to_string(e->l, cb);
            cb(e, ", ", WalkType::NORMAL);
            rdil::to_string(e->r, cb);
            cb(e, ")", WalkType::NORMAL);
            break;

        case RDIL_NOT:
            cb(e, rdil::text_op(e), WalkType::NORMAL);
            rdil::wrap_walk(e->u, cb);
            break;

        case RDIL_IF:
            cb(e, "if", WalkType::MNEMONIC);
            cb(e, " ", WalkType::NORMAL);
            cb(e, "(", WalkType::NORMAL);
            rdil::to_string(e->cond, cb);
            cb(e, ") ", WalkType::NORMAL);
            rdil::to_string(e->t, cb);
            cb(e, " ", WalkType::WHITESPACE);
            cb(e, "else", WalkType::MNEMONIC);
            cb(e, " ", WalkType::WHITESPACE);
            rdil::to_string(e->f, cb);
            break;

        case RDIL_RET:
            cb(e, "ret", WalkType::MNEMONIC);
            cb(e, "(", WalkType::NORMAL);
            rdil::to_string(e->cond, cb);
            cb(e, ")", WalkType::NORMAL);
            break;

        case RDIL_CNST:
        case RDIL_VAR:
        case RDIL_REG:
        case RDIL_SYM: cb(e, std::string{}, WalkType::NORMAL); break;

        case RDIL_COPY:
            rdil::to_string(e->dst, cb);
            cb(e, "=", WalkType::NORMAL);
            rdil::to_string(e->src, cb);
            break;

        case RDIL_INT:
            cb(e, "int", WalkType::NORMAL);
            cb(e, " ", WalkType::NORMAL);
            rdil::to_string(e->u, cb);
            break;

        case RDIL_CALL:
            rdil::to_string(e->u, cb);
            cb(e, "()", WalkType::NORMAL);
            break;

        case RDIL_GOTO:
            cb(e, "goto", WalkType::MNEMONIC);
            cb(e, "(", WalkType::NORMAL);
            rdil::to_string(e->u, cb);
            cb(e, ")", WalkType::NORMAL);
            break;

        case RDIL_MEM:
            cb(e, "[", WalkType::NORMAL);
            rdil::to_string(e->u, cb);
            cb(e, "]", WalkType::NORMAL);
            break;

        case RDIL_PUSH:
            cb(e, "push", WalkType::MNEMONIC);
            cb(e, "(", WalkType::NORMAL);
            rdil::to_string(e->u, cb);
            cb(e, ")", WalkType::NORMAL);
            break;

        case RDIL_POP:
            rdil::to_string(e->u, cb);
            cb(e, "=", WalkType::NORMAL);
            cb(e, "pop()", WalkType::MNEMONIC);
            break;

        case RDIL_UNKNOWN: cb(e, "unknown", WalkType::MNEMONIC); break;
        case RDIL_NOP: cb(e, "nop", WalkType::MNEMONIC); break;
        default: cb(e, "???", WalkType::NORMAL); break;
    }
}

void get_text_impl(const ILExpr* e, std::string& res) {
    rdil::to_string(e, [&res](const ILExpr* expr, const std::string& s,
                              WalkType) {
        Context* ctx = state::context;

        switch(expr->op) {
            case RDIL_CNST: res += ctx->to_hex(expr->u_value, -1); break;
            case RDIL_VAR: res += ctx->to_hex(expr->address); break;

            case RDIL_REG: {
                res +=
                    ctx->processor->getregistername(ctx->processor, expr->reg);
                break;
            }

            case RDIL_SYM: res += expr->sym; break;
            default: res += s; break;
        }
    });
}

} // namespace

std::string_view get_op_name(usize t) {
    static const std::unordered_map<usize, std::string_view> NAMES = {
        RDIL_N(RDIL_UNKNOWN), RDIL_N(RDIL_NOP),  RDIL_N(RDIL_REG),
        RDIL_N(RDIL_SYM),     RDIL_N(RDIL_CNST), RDIL_N(RDIL_VAR),
        RDIL_N(RDIL_ADD),     RDIL_N(RDIL_SUB),  RDIL_N(RDIL_MUL),
        RDIL_N(RDIL_DIV),     RDIL_N(RDIL_MOD),  RDIL_N(RDIL_AND),
        RDIL_N(RDIL_OR),      RDIL_N(RDIL_XOR),  RDIL_N(RDIL_NOT),
        RDIL_N(RDIL_LSL),     RDIL_N(RDIL_LSR),  RDIL_N(RDIL_ASL),
        RDIL_N(RDIL_ASR),     RDIL_N(RDIL_ROL),  RDIL_N(RDIL_ROR),
        RDIL_N(RDIL_MEM),     RDIL_N(RDIL_COPY), RDIL_N(RDIL_GOTO),
        RDIL_N(RDIL_CALL),    RDIL_N(RDIL_RET),  RDIL_N(RDIL_IF),
        RDIL_N(RDIL_EQ),      RDIL_N(RDIL_NE),   RDIL_N(RDIL_PUSH),
        RDIL_N(RDIL_POP),     RDIL_N(RDIL_INT),
    };

    auto it = NAMES.find(t);
    return (it != NAMES.end()) ? it->second : std::string_view{"invalid"};
}

usize get_op_type(std::string_view id) {
    static const std::unordered_map<std::string_view, usize> IDS = {
        {"unknown", RDIL_UNKNOWN}, {"nop", RDIL_NOP},   {"reg", RDIL_REG},
        {"sym", RDIL_SYM},         {"cnst", RDIL_CNST}, {"var", RDIL_VAR},
        {"add", RDIL_ADD},         {"sub", RDIL_SUB},   {"mul", RDIL_MUL},
        {"div", RDIL_DIV},         {"mod", RDIL_MOD},   {"and", RDIL_AND},
        {"or", RDIL_OR},           {"xor", RDIL_XOR},   {"not", RDIL_NOT},
        {"lsl", RDIL_LSL},         {"lsr", RDIL_LSR},   {"asl", RDIL_ASL},
        {"asr", RDIL_ASR},         {"rol", RDIL_ROL},   {"ror", RDIL_ROR},
        {"mem", RDIL_MEM},         {"copy", RDIL_COPY}, {"goto", RDIL_GOTO},
        {"call", RDIL_CALL},       {"ret", RDIL_RET},   {"eq", RDIL_EQ},
        {"ne", RDIL_NE},           {"push", RDIL_PUSH}, {"pop", RDIL_POP},
        {"int", RDIL_INT},
    };

    auto it = IDS.find(id);
    if(it != IDS.end())
        return it->second;
    return RDIL_INVALID;
}

std::string get_text(const ILExpr* e) {
    std::string res;
    rdil::get_text_impl(e, res);
    return res;
}

std::string get_format(const ILExpr* e) {
    if(!e)
        return {};

    std::string fmt;
    if(!rdil::get_format_impl(e, fmt))
        return {};
    return fmt;
}

void generate(const Function& f, ILExprList& res) {
    rdil::generate(f, res, std::numeric_limits<usize>::max());
}

void generate(const Function& f, ILExprList& res, usize maxn) {
    const Context* ctx = state::context;
    const auto& mem = state::context->memory;
    const RDProcessor* p = ctx->processor;
    assume(p);

    for(const Function::BasicBlock& bb : f.blocks) {
        for(MIndex idx = bb.start; idx <= bb.end;) {
            res.currentindex = idx;

            auto address = ctx->index_to_address(idx);
            assume(address.has_value());

            RDInstruction instr{.address = *address};
            if(p->decode)
                p->decode(p, &instr);

            if(!instr.length || !p->lift ||
               !p->lift(p, api::to_c(&res), &instr)) {
                res.append(res.expr_unknown());
            }

            if(res.size() >= maxn)
                return;

            if(auto nextidx = mem->get_next(idx); nextidx) {
                assume(*nextidx > idx);
                idx = *nextidx;
            }
            else
                break;
        }
    }
}

void render(const ILExpr* e, Renderer& renderer) {
    rdil::to_string(
        e, [&](const ILExpr* expr, std::string_view s, WalkType wt) {
            if(wt == WalkType::MNEMONIC) {
                switch(expr->op) {
                    case RDIL_GOTO: renderer.chunk(s, THEME_JUMP); break;
                    case RDIL_RET: renderer.chunk(s, THEME_RET); break;
                    case RDIL_NOP: renderer.chunk(s, THEME_NOP); break;

                    case RDIL_UNKNOWN: {
                        renderer.chunk(s, THEME_NOP)
                            .ws()
                            .chunk("{")
                            .instr()
                            .chunk("}");
                        break;
                    }

                    default: renderer.chunk(s, THEME_DEFAULT); break;
                }

                return;
            }

            switch(expr->op) {
                case RDIL_CNST:
                    renderer.constant(expr->u_value, THEME_CONSTANT);
                    break;

                case RDIL_VAR: renderer.addr(expr->address); break;
                case RDIL_REG: renderer.reg(expr->reg); break;
                case RDIL_SYM: renderer.chunk(expr->sym, THEME_ADDRESS); break;
                default: renderer.chunk(s); break;
            }
        });
}

} // namespace redasm::rdil
