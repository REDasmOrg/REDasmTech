#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

namespace {

void do_enqueue(std::deque<MIndex>& q, MIndex index) {
    if(q.empty() || q.front() != index)
        q.push_front(index);
}

} // namespace

void Emulator::add_ref(MIndex toidx, usize type) { // NOLINT
    state::context->add_ref(this->pc, toidx, type);
}

void Emulator::enqueue_flow(MIndex index) { do_enqueue(m_qflow, index); }
void Emulator::enqueue_jump(MIndex index) { do_enqueue(m_qjump, index); }
void Emulator::enqueue_call(MIndex index) { do_enqueue(m_qcall, index); }

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

    // Weak bytes can be overriden
    if(b.is_weak() && !b.is_unknown()) {
        usize len = ctx->memory->get_length(idx);
        ctx->memory->unset_n(idx, len);
    }

    if(b.is_unknown()) {
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
            if(p->emulate)
                p->emulate(p, api::to_c(this), &instr);
            mem->set_n(idx, instr.length, BF_CODE);

            if(instr.features & INSTR_JUMP)
                mem->set(idx, BF_JUMP);
            if(instr.features & INSTR_CALL)
                mem->set(idx, BF_CALL);
        }
    }
    else
        ctx->add_problem(idx, "location is not unknown");
}

bool Emulator::has_pending() const {
    return state::context->processor->emulate &&
           (!m_qflow.empty() || !m_qjump.empty() || !m_qcall.empty());
}

} // namespace redasm
