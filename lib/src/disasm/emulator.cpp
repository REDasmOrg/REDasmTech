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
        v.insert(it, idx);
}

} // namespace

void Emulator::enqueue(usize idx) {
    if(idx < state::context->memory->size())
        m_pending.push_front(idx);
}

void Emulator::schedule(usize idx) {
    if(idx < state::context->memory->size())
        m_pending.push_back(idx);
}

tl::optional<RDAddress> Emulator::decode(usize idx) {
    const Segment* s = this->get_segment(idx);

    if(!s || !(s->type & SEGMENTTYPE_HASCODE))
        return tl::nullopt;

    Context* ctx = state::context;
    Memory* m = ctx->memory.get();

    if(Byte b = m->at(idx); !b.has_byte() || !b.is_unknown())
        return tl::nullopt;

    const RDProcessor* p = ctx->processor;
    assume(p);

    if(!p->emulate)
        return tl::nullopt;

    m_currindex = idx;

    auto address = ctx->index_to_address(idx);
    assume(address.has_value());

    if(usize sz = p->emulate(p, *address, api::to_c(this)); sz)
        m->set_code(idx, sz);

    return tl::nullopt;
}

const Segment* Emulator::get_segment(usize idx) {
    if(m_segment && (idx >= m_segment->index && idx < m_segment->endindex))
        return m_segment;

    m_segment = state::context->index_to_segment(idx);
    return m_segment;
}

void Emulator::check_location(RDAddress address) {
    auto idx = state::context->address_to_index(address);
    if(!idx)
        return;

    Byte b = state::context->memory->at(*idx);
    if(!b.is_unknown())
        return;

    const Segment* s = this->get_segment(*idx);
    if(!s)
        return;

    if(s->type & SEGMENTTYPE_HASDATA) {
    }
}

void Emulator::add_coderef(usize idx, usize cr) {
    if(idx >= state::context->memory->size())
        return;

    Context* ctx = state::context;

    const Segment* s = ctx->index_to_segment(idx);
    if(!s && !(s->type & SEGMENTTYPE_HASCODE))
        return;

    AddressDetail& d = ctx->database.get_detail(idx);

    switch(cr) {
        case CR_CALL:
            ctx->memory->at(m_currindex).set(BF_CALL);
            ctx->memory->at(idx).set(BF_FUNCTION);
            this->schedule(idx);
            sorted_unique_insert(d.calls, idx);
            break;

        case CR_JUMP:
            ctx->memory->at(m_currindex).set(BF_JUMP);
            ctx->memory->at(idx).set(BF_JUMPDST);
            this->schedule(idx);
            sorted_unique_insert(d.jumps, idx);
            break;

        case CR_FLOW: {
            ctx->memory->at(m_currindex).set(BF_FLOW);
            this->enqueue(idx);
            d.flow = idx;
            return;
        }

        default: unreachable; return;
    }

    AddressDetail& refd = ctx->database.get_detail(idx);
    sorted_unique_insert(refd.refs, m_currindex);
    ctx->memory->at(idx).set(BF_REFS);
}

void Emulator::add_dataref(usize idx, usize dr) {
    if(idx >= state::context->memory->size())
        return;

    Context* ctx = state::context;

    stringfinder::classify(idx).map([idx, ctx](const RDStringResult& x) {
        ctx->memory->set_unknown(idx, x.totalsize);
        ctx->set_type(idx, x.type);
    });

    AddressDetail& refd = ctx->database.get_detail(idx);
    sorted_unique_insert(refd.refs, m_currindex);
}

void Emulator::next() {
    if(m_pending.empty())
        return;

    usize idx = m_pending.front();
    m_pending.pop_front();
    this->decode(idx);
}

bool Emulator::has_next() const {
    return state::context->processor->emulate && !m_pending.empty();
}

} // namespace redasm
