#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

namespace {

void do_enqueue(std::deque<MIndex>& q, MIndex idx) {
    if(q.empty() || q.front() != idx)
        q.push_front(idx);
}

} // namespace

// Emulator::~Emulator() {
//     const Context* ctx = state::context;
//
//     if(m_state && ctx->processor->state.fini)
//         ctx->processor->state.fini(ctx->processor, m_state);
// }

// void Emulator::activate() {
//     const Context* ctx = state::context;
//
//     if(ctx->processor->state.init)
//         m_state = ctx->processor->state.init(ctx->processor);
// }

void Emulator::flow(MIndex index) {
    const Segment* fromseg = state::context->index_to_segment(this->current);
    assume(fromseg);
    assume(fromseg->type & SEG_HASCODE);

    // Avoid inter-segment flow
    if(index >= fromseg->index && index < fromseg->endindex) {
        state::context->memory->set(this->current, BF_FLOW);
        this->enqueue_flow(index);
    }
}

bool Emulator::set_typename(MIndex idx, typing::FullTypeName tname) {
    typing::ParsedType pt = state::context->types.parse(tname);
    return this->set_type(idx, pt.to_type());
}

bool Emulator::set_type(MIndex idx, RDType type) {
    const Context* ctx = state::context;
    if(idx >= ctx->memory->size())
        return false;

    const typing::TypeDef* newtd = ctx->types.get_typedef(type);
    usize newsz = newtd->size * std::max<usize>(type.n, 1);

    if(idx + newsz >= ctx->memory->size())
        return false;

    auto it = m_ptypes.find(idx);

    // if(it == m_ptypes.end()) {
    // m_ptypes[idx] = type;
    // return true;
    // }

    // const typing::TypeDef* oldtd = ctx->types.get_typedef(it->second);
    // usize oldsz = oldtd->size * std::max<usize>(it->second.n, 1);

    // if(newsz < oldsz) {
    // m_ptypes[idx] = type;
    // return true;
    // }

    return false;
}

void Emulator::add_ref(MIndex toidx, usize type) { // NOLINT
    state::context->add_ref(this->current, toidx, type);
}

void Emulator::enqueue_flow(MIndex index) { do_enqueue(m_qflow, index); }

void Emulator::enqueue_jump(MIndex index) {
    // TODO(davide): Snapshot State
    do_enqueue(m_qjump, index);
}

void Emulator::enqueue_call(MIndex index) {
    // TODO(davide): Snapshot State
    do_enqueue(m_qcall, index);
}

void Emulator::tick() {
    MIndex idx;
    Context* ctx = state::context;
    auto& mem = ctx->memory;

    if(!m_qflow.empty()) {
        idx = m_qflow.front();
        m_qflow.pop_front();
    }
    else if(!m_qjump.empty()) {
        idx = m_qjump.front();
        m_qjump.pop_front();
    }
    else if(!m_qcall.empty()) {
        idx = m_qcall.front();
        m_qcall.pop_front();
    }
    else
        return;

    const RDProcessor* p = ctx->processor;
    assume(p);

    Byte& b = ctx->memory->at(idx);

    if(!b.is_code()) {
        this->current = idx;

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
            mem->unset_n(idx, instr.length);

            if(p->emulate) {
                p->emulate(p, api::to_c(this), &instr);

                if(!(instr.features & IF_STOP))
                    this->flow(idx + instr.length);
            }

            mem->set_n(idx, instr.length, BF_CODE);

            if(instr.features & IF_JUMP)
                mem->set(idx, BF_JUMP);
            if(instr.features & IF_CALL)
                mem->set(idx, BF_CALL);
        }
    }
}

void Emulator::tick_type() {
    assume(!m_ptypes.empty());
    auto [index, type] = *m_ptypes.begin();
    m_ptypes.erase(m_ptypes.begin());
    this->current = index;

    Context* ctx = state::context;
    // const typing::TypeDef* td = ctx->types.get_typedef(type);
    // usize sz = td->size * std::max<usize>(type.n, 1);

    // for(usize i = index; i < index + sz; i++) {
    // Byte b = ctx->memory->at(i);
    // if(b.is_code() || b.has(BF_SEGMENT))
    // return;
    // }

    // TODO(davide): Move string classifier here?
    // ctx->set_type(index, type, ST_WEAK);
}

bool Emulator::has_pending_code() const {
    return state::context->processor->emulate &&
           (!m_qflow.empty() || !m_qjump.empty() || !m_qcall.empty());
}

bool Emulator::has_pending_types() const { return !m_ptypes.empty(); }

} // namespace redasm
