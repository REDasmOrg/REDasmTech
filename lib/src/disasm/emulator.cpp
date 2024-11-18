#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../memory/stringfinder.h"
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

bool Emulator::enqueue(MIndex idx) {
    if(idx >= state::context->memory->size())
        return false;

    const Segment* s = this->get_segment(idx);
    if(!s || !(s->type & SEGMENTTYPE_HASCODE))
        return false;

    // Already decoded
    if(state::context->memory->at(idx).has(BF_INSTR))
        return true;

    // Don't enqueue duplicates
    if(!m_pending.empty() && m_pending.front() == idx)
        return true;

    m_pending.push_front(idx);
    return true;
}

bool Emulator::schedule(MIndex idx) {
    if(idx >= state::context->memory->size())
        return false;

    const Segment* s = this->get_segment(idx);
    if(!s || !(s->type & SEGMENTTYPE_HASCODE))
        return false;

    // Already decoded
    if(state::context->memory->at(idx).has(BF_INSTR))
        return true;

    // Don't schedule duplicates
    if(!m_pending.empty() && m_pending.back() == idx)
        return true;

    m_pending.push_back(idx);
    return true;
}

void Emulator::decode(MIndex idx) {
    Context* ctx = state::context;
    auto& mem = ctx->memory;

    if(Byte b = mem->at(idx); !b.has_byte() || b.is_strong())
        return;

    const RDProcessor* p = ctx->processor;
    assume(p);

    if(!p->emulate)
        return;

    m_currindex = idx;

    auto address = ctx->index_to_address(idx);
    assume(address.has_value());
    mem->unset(idx);

    RDInstructionDetail d = {
        .address = *address,
        .size = 0,
        .type = IT_NONE,
    };

    p->emulate(p, api::to_c(this), &d);

    if(!d.size)
        return;

    mem->set_n(idx, d.size, BF_INSTR | BF_WEAK);

    if(d.type & IT_JUMP) {
        ctx->database.check_detail(idx); // Initialize DB Entry
        ctx->memory->set(idx, BF_JUMP);
    }

    if(d.type & IT_CALL) {
        ctx->database.check_detail(idx); // Initialize DB Entry
        ctx->memory->set(idx, BF_CALL);
    }

    if(!(d.type & IT_STOP) && this->enqueue(idx + d.size)) {
        AddressDetail& ad = ctx->database.check_detail(idx);
        ad.flow = idx + d.size;
        ctx->memory->set(idx, BF_FLOW);
    }
}

const Segment* Emulator::get_segment(MIndex idx) {
    if(m_segment && (idx >= m_segment->index && idx < m_segment->endindex))
        return m_segment;

    m_segment = state::context->index_to_segment(idx);
    return m_segment;
}

void Emulator::add_ref(MIndex idx, usize type) {
    Context* ctx = state::context;
    if(idx >= ctx->memory->size())
        return;

    AddressDetail& dto = ctx->database.check_detail(m_currindex);

    if(type & REF_CALL) {
        if(ctx->set_function(idx))
            this->schedule(idx);
        sorted_unique_insert(dto.calls, {.index = idx, .type = type});
    }

    if(type & REF_JUMP) {
        if(this->schedule(idx))
            ctx->memory->set(idx, BF_JUMPDST);
        sorted_unique_insert(dto.jumps, {.index = idx, .type = type});
    }

    // Data reference
    if(!(type & (REF_CALL | REF_JUMP))) {
        if(!(type & REF_INDIRECT) && !ctx->memory->at(idx).is_strong()) {
            stringfinder::classify(idx).map([&](const RDStringResult& x) {
                ctx->memory->unset_n(idx, x.totalsize);
                ctx->set_type(idx, x.type);
                ctx->memory->set(idx, BF_WEAK);
            });
        }

        sorted_unique_insert(dto.refsto, {.index = idx, .type = type});
        ctx->memory->set(m_currindex, BF_REFSTO);
    }

    AddressDetail& dby = ctx->database.check_detail(idx);
    sorted_unique_insert(dby.refs, {.index = m_currindex, .type = type});
    ctx->memory->set_flags(idx, BF_REFS, !dby.refs.empty());
}

void Emulator::set_type(MIndex idx, std::string_view tname) {
    Context* ctx = state::context;

    if(idx < ctx->memory->size() && ctx->set_type(idx, tname))
        ctx->memory->set(idx, BF_WEAK);
}

void Emulator::next() {
    if(m_pending.empty())
        return;

    usize idx = m_pending.front();
    m_pending.pop_front();
    this->decode(idx);
}

tl::optional<RDAddress> Emulator::get_next_address() const {
    if(m_pending.empty())
        return tl::nullopt;

    return state::context->index_to_address(m_pending.front());
}

bool Emulator::has_next() const {
    return state::context->processor->emulate && !m_pending.empty();
}

} // namespace redasm
