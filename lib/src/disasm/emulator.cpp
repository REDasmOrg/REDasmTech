#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../memory/stringfinder.h"
#include "../state.h"
#include <algorithm>

namespace redasm {

namespace {

void sorted_unique_insert(std::vector<usize>& v, usize idx) {
    auto it = std::lower_bound(v.begin(), v.end(), idx);
    if(it == v.end() || (*it != idx))
        v.emplace(it, idx);
}

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

    if(usize sz = p->emulate(p, *address, api::to_c(this)); sz)
        mem->set_n(idx, sz, BF_INSTR | BF_WEAK);
}

const Segment* Emulator::get_segment(MIndex idx) {
    if(m_segment && (idx >= m_segment->index && idx < m_segment->endindex))
        return m_segment;

    m_segment = state::context->index_to_segment(idx);
    return m_segment;
}

void Emulator::add_coderef(MIndex idx, usize cr) {
    Context* ctx = state::context;
    if(idx >= ctx->memory->size())
        return;

    AddressDetail& dto = ctx->database.get_detail(m_currindex);

    switch(cr) {
        case CR_CALL: {
            ctx->memory->set(m_currindex, BF_CALL);
            if(ctx->set_function(idx) && this->schedule(idx))
                sorted_unique_insert(dto.calls, idx);
            break;
        }

        case CR_JUMP: {
            ctx->memory->set(m_currindex, BF_JUMP);
            if(this->schedule(idx)) {
                ctx->memory->set(idx, BF_JUMPDST);
                sorted_unique_insert(dto.jumps, idx);
            }
            break;
        }

        case CR_FLOW: {
            if(this->enqueue(idx)) {
                ctx->memory->set(m_currindex, BF_FLOW);
                dto.flow = idx;
            }
            return;
        }

        default: unreachable; return;
    }

    AddressDetail& dby = ctx->database.get_detail(idx);
    sorted_unique_insert(dby.refs, {m_currindex, cr});
    ctx->memory->set_flags(idx, BF_REFS, !dby.refs.empty());
}

void Emulator::add_dataref(MIndex idx, usize dr) {
    if(idx >= state::context->memory->size())
        return;

    Context* ctx = state::context;

    stringfinder::classify(idx).map([idx, ctx](const RDStringResult& x) {
        ctx->memory->unset_n(idx, x.totalsize);
        ctx->set_type(idx, x.type);
        ctx->memory->at(idx).set(BF_WEAK);
    });

    AddressDetail& dto = ctx->database.get_detail(m_currindex);
    sorted_unique_insert(dto.refsto, {idx, dr});
    ctx->memory->at(m_currindex).set(BF_REFSTO);

    AddressDetail& dby = ctx->database.get_detail(idx);
    sorted_unique_insert(dby.refs, {m_currindex, dr});
    ctx->memory->at(idx).set_flag(BF_REFS, !dby.refs.empty());
}

void Emulator::set_type(MIndex idx, std::string_view tname) {
    Context* ctx = state::context;

    if(idx >= ctx->memory->size())
        return;

    if(ctx->set_type(idx, tname))
        ctx->memory->at(idx).set(BF_WEAK);
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
