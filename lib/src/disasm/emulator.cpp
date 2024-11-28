#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"
#include <algorithm>

namespace redasm {

namespace {

void sorted_unique_insert(std::vector<AddressDetail::Ref>& v,
                          AddressDetail::Ref r) {
    auto it = std::lower_bound(
        v.begin(), v.end(), r,
        [](const auto& a, const auto& b) { return a.index < b.index; });

    if(it == v.end() || (it->index != r.index))
        v.insert(it, r);
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
    if(toidx >= ctx->memory->size())
        return;

    // stringfinder::classify(toidx).map([&](const RDStringResult& x) {
    //     ctx->memory->unset_n(toidx, x.totalsize);
    //     ctx->set_type(toidx, x.type);
    //     ctx->memory->set(toidx, BF_WEAK);
    // });

    switch(type) {
        case DR_READ:
        case DR_WRITE:
        case DR_ADDRESS: break;

        case CR_FLOW: {
            this->qflow.push_back({
                .from = AddressDetail::Ref{.index = fromidx, .type = type},
                .index = toidx,
            });
            break;
        }

        case CR_JUMP: {
            this->qjump.push_back({
                .from = AddressDetail::Ref{.index = fromidx, .type = type},
                .index = toidx,
            });
            break;
        }

        case CR_CALL: {
            this->qcall.push_back({
                .from = AddressDetail::Ref{.index = fromidx, .type = type},
                .index = toidx,
            });
            break;
        }

        default: unreachable;
    }
}

bool Emulator::check_qitem(const Emulator::QueueItem& item,
                           const std::string& errmsg) const {
    const Segment* s = this->get_segment(item.index);

    if(!s) {
        state::context->add_problem(item.index, errmsg);
        return false;
    }

    return true;
}

void Emulator::tick() {
    QueueItem curr;
    Context* ctx = state::context;
    auto& mem = ctx->memory;

    if(!this->qflow.empty()) {
        curr = this->qflow.front();
        this->qflow.pop_front();

        if(!this->check_qitem(curr, fmt::format("Invalid FLOW address")))
            return;

        if(curr.from) {
            AddressDetail& dfrom = ctx->database.check_detail(curr.from->index);
            dfrom.flow = curr.index;

            // Apply 'flow' to previous instruction
            ctx->memory->set(curr.from->index, BF_FLOW);
        }
    }
    else if(!this->qjump.empty()) {
        curr = this->qjump.front();
        this->qjump.pop_front();

        if(!this->check_qitem(curr, fmt::format("Invalid JUMP address")))
            return;

        AddressDetail& dto = ctx->database.check_detail(curr.index);

        if(curr.from) {
            AddressDetail& dfrom = ctx->database.check_detail(curr.from->index);
            sorted_unique_insert(
                dfrom.jumps, {.index = curr.index, .type = curr.from->type});
            sorted_unique_insert(dto.refs, *curr.from);
            mem->set(curr.from->index, BF_JUMP);
        }

        mem->set_flags(curr.index, BF_REFS, !dto.refs.empty());
        mem->set(curr.index, BF_JUMPDST);
    }
    else if(!qcall.empty()) {
        curr = qcall.front();
        qcall.pop_front();

        if(!this->check_qitem(curr, fmt::format("Invalid CALL address")))
            return;

        if(!ctx->set_function(curr.index)) {
            ctx->add_problem(curr.index,
                             fmt::format("Function creation failed"));
            return;
        }

        AddressDetail& dto = ctx->database.check_detail(curr.index);

        if(curr.from) {
            AddressDetail& dfrom = ctx->database.check_detail(curr.from->index);
            sorted_unique_insert(
                dfrom.jumps, {.index = curr.index, .type = curr.from->type});
            sorted_unique_insert(dto.refs, *curr.from);
            mem->set(curr.from->index, BF_CALL);
        }

        mem->set_flags(curr.index, BF_REFS, !dto.refs.empty());
        mem->set(curr.index, BF_FUNCTION);
    }
    else
        return;

    assume(m_currsegment);
    assume(curr.index < ctx->memory->size());

    if(!(m_currsegment->type & SEG_HASCODE)) {
        ctx->add_problem(
            curr.index,
            fmt::format("Trying to execute in non-code segment '{}'",
                        m_currsegment->name));
        return;
    }

    const RDProcessor* p = ctx->processor;
    assume(p);
    assume(p->emulate);

    if(ctx->memory->at(curr.index).is_unknown()) {
        this->pc = curr.index;

        auto address = ctx->index_to_address(curr.index);
        assume(address.has_value());

        RDInstruction instr = {
            .address = *address,
        };

        if(p->decode)
            p->decode(p, &instr);
        else {
            ctx->add_problem(
                curr.index,
                fmt::format("decode() not implemented for processor '{}'",
                            p->name));
            return;
        }

        if(instr.length) {
            p->emulate(p, api::to_c(this), &instr);
            mem->set_n(curr.index, instr.length, BF_INSTR);
        }
    }
}

bool Emulator::has_pending() const {
    return state::context->processor->emulate &&
           (!this->qflow.empty() || !this->qjump.empty() ||
            !this->qcall.empty());
}

} // namespace redasm
