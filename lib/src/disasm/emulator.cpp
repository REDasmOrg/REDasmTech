#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

void Emulator::add_ref(MIndex toidx, usize type) { // NOLINT
    state::context->add_ref(this->pc, toidx, type);
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
            mem->set_n(idx, instr.length, BF_CODE);

            if(instr.features & INSTR_JUMP)
                mem->set(idx, BF_JUMP);
            if(instr.features & INSTR_CALL)
                mem->set(idx, BF_CALL);
        }
    }
}

bool Emulator::has_pending() const {
    return state::context->processor->emulate &&
           (!this->qflow.empty() || !this->qjump.empty() ||
            !this->qcall.empty());
}

} // namespace redasm
