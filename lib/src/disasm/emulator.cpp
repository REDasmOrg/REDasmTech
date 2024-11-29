#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"
#include <algorithm>

namespace redasm {

namespace {

void sortedunique_insert(std::vector<MIndex>& v, MIndex r) {
    auto it =
        std::lower_bound(v.begin(), v.end(), r,
                         [](const auto& a, const auto& b) { return a < b; });

    if(it == v.end() || (*it != r))
        v.insert(it, r);
}

void sortedunique_insert(std::vector<AddressDetail::Ref>& v,
                         AddressDetail::Ref r) {
    auto it = std::lower_bound(
        v.begin(), v.end(), r,
        [](const auto& a, const auto& b) { return a.index < b.index; });

    if(it == v.end() || (it->index != r.index))
        v.insert(it, r);
}

std::string_view get_refname(usize reftype) {
    switch(reftype) {
        case DR_READ: return "READ";
        case DR_WRITE: return "WRITE";
        case DR_ADDRESS: return "ADDRESS";
        case CR_FLOW: return "FLOW";
        case CR_JUMP: return "JUMP";
        case CR_CALL: return "CALL";
        default: break;
    }

    unreachable;
}

void add_noncodeproblem(const Segment* s, MIndex index, usize type) {
    state::context->add_problem(
        index, fmt::format("Trying to {} in non-code segment '{}'",
                           get_refname(type), s->name));
}

} // namespace

const Segment* Emulator::get_segment(MIndex idx) const {
    if(m_currsegment &&
       (idx >= m_currsegment->index && idx < m_currsegment->endindex))
        return m_currsegment;

    m_currsegment = state::context->index_to_segment(idx);
    return m_currsegment;
}

void Emulator::add_ref(MIndex fromidx, MIndex toidx, usize type) {
    Context* ctx = state::context;
    const Segment* s = this->get_segment(toidx);

    if(!s) {
        ctx->add_problem(
            toidx, fmt::format("Invalid {} reference", get_refname(type)));
        return;
    }

    // stringfinder::classify(toidx).map([&](const RDStringResult& x) {
    //     ctx->memory->unset_n(toidx, x.totalsize);
    //     ctx->set_type(toidx, x.type);
    //     ctx->memory->set(toidx, BF_WEAK);
    // });

    auto& mem = ctx->memory;
    AddressDetail& dfrom = ctx->database.check_detail(fromidx);
    AddressDetail& dto = ctx->database.check_detail(toidx);

    switch(type) {
        case DR_READ:
        case DR_WRITE:
        case DR_ADDRESS: {
            sortedunique_insert(dto.refs, {.index = fromidx, .type = type});
            break;
        }

        case CR_FLOW: {
            if(s->type & SEG_HASCODE) {
                this->qflow.push_back(toidx);
                dfrom.flow = toidx;
                ctx->memory->set(fromidx, BF_FLOW);
            }
            else
                add_noncodeproblem(s, toidx, type);
            break;
        }

        case CR_JUMP: {
            if(s->type & SEG_HASCODE) {
                this->qjump.push_back(toidx);
                sortedunique_insert(dfrom.jumps, toidx);
                sortedunique_insert(dto.refs, {.index = fromidx, .type = type});
                mem->set(fromidx, BF_JUMP);
                mem->set(toidx, BF_JUMPDST);
                mem->set_flags(toidx, BF_REFS, !dto.refs.empty());
            }
            else
                add_noncodeproblem(s, toidx, type);
            break;
        }

        case CR_CALL: {
            if(s->type & SEG_HASCODE) {
                this->qcall.push_back(toidx);
                sortedunique_insert(dfrom.calls, toidx);
                sortedunique_insert(dto.refs, {.index = fromidx, .type = type});
                mem->set(fromidx, BF_CALL);
                mem->set(toidx, BF_FUNCTION);
                mem->set_flags(toidx, BF_REFS, !dto.refs.empty());
            }
            else
                add_noncodeproblem(s, toidx, type);
            break;
        }

        default: unreachable;
    }
}

void Emulator::tick() {
    MIndex idx;
    Context* ctx = state::context;
    auto& mem = ctx->memory;

    if(!this->qflow.empty()) {
        idx = this->qflow.front();
        this->qflow.pop_front();
    }
    else if(!this->qjump.empty()) {
        idx = this->qjump.front();
        this->qjump.pop_front();
    }
    else if(!qcall.empty()) {
        idx = qcall.front();
        qcall.pop_front();
    }
    else
        return;

    const RDProcessor* p = ctx->processor;
    assume(p);
    assume(p->emulate);

    if(ctx->memory->at(idx).is_unknown()) {
        this->pc = idx;

        auto address = ctx->index_to_address(idx);
        assume(address.has_value());

        RDInstruction instr = {
            .address = *address,
        };

        if(p->decode)
            p->decode(p, &instr);
        else {
            ctx->add_problem(
                idx, fmt::format("decode() not implemented for processor '{}'",
                                 p->name));
            return;
        }

        if(instr.length) {
            p->emulate(p, api::to_c(this), &instr);
            mem->set_n(idx, instr.length, BF_INSTR);
        }
    }
}

bool Emulator::has_pending() const {
    return state::context->processor->emulate &&
           (!this->qflow.empty() || !this->qjump.empty() ||
            !this->qcall.empty());
}

} // namespace redasm
