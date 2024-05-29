#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"
#include <algorithm>

namespace redasm {

namespace {

void link_refs(usize idx, RDAddress refaddress) {
    AddressDetail& d = state::context->database.get_detail(idx);

    state::context->address_to_index(refaddress).map([&](usize x) {
        auto it = std::lower_bound(d.refs.begin(), d.refs.end(), x);
        if(it == d.refs.end() || (*it != x))
            d.refs.insert(it, x);
    });
}

void link_next(usize idx, RDAddress nextaddress) {
    AddressDetail& d = state::context->database.get_detail(idx);

    state::context->address_to_index(nextaddress).map([&](usize x) {
        auto it = std::lower_bound(d.next.begin(), d.next.end(), x);
        if(it == d.next.end() || (*it != x))
            d.next.insert(it, x);
    });
}

} // namespace

void Emulator::enqueue(RDAddress address) {
    if(state::context->is_address(address))
        m_pending.push_front(address);
}

void Emulator::schedule(RDAddress address) {
    if(state::context->is_address(address))
        m_pending.push_back(address);
}

tl::optional<RDAddress> Emulator::decode(EmulateResult&& r) {
    if(!state::context->is_address(r.address))
        return tl::nullopt;

    auto idx = state::context->address_to_index(r.address);
    assume(idx.has_value());

    if(const Segment* s = this->get_segment(*idx);
       !s || !(s->type & SEGMENTTYPE_HASCODE))
        return tl::nullopt;

    Memory* m = state::context->memory.get();

    if(Byte b = m->at(*idx); !b.has_byte() || !b.is_unknown())
        return tl::nullopt;

    const RDProcessor* p = state::context->processor;
    assume(p);

    p->emulate(p, api::to_c(&r));

    if(r.size) {
        m->set_code(*idx, r.size);

        if(r.delayslot) {
            // TODO(davide): Handle delay slots
        }

        this->process_result(r);

        if(r.canflow) {
            RDAddress nextaddress = r.address + r.size;
            link_next(*idx, nextaddress);
            return nextaddress;
        }
    }

    return tl::nullopt;
}

void Emulator::process_result(const EmulateResult& r) {
    Memory* memory = state::context->memory.get();

    for(const auto& [forktype, res] : r.results) {
        auto idx = state::context->address_to_index(res.address);
        if(!idx)
            continue;

        switch(forktype) {
            case EmulateResult::REF:
            case EmulateResult::REF_DATA:
            case EmulateResult::REF_STRING:
                // case EmulateResult::TABLE: break;
                link_refs(*idx, r.address);
                break;

            case EmulateResult::BRANCH:
            case EmulateResult::BRANCH_TRUE:
                link_refs(*idx, r.address);
                memory->at(*idx).set(BF_BRANCH);
                this->schedule(res.address);
                break;

            case EmulateResult::BRANCH_FALSE:
                link_refs(*idx, r.address);
                this->enqueue(res.address);
                break;

                // case EmulateResult::BRANCH_TABLE: break;

            case EmulateResult::CALL:
                link_refs(*idx, r.address);
                state::context->memory->at(*idx).set(BF_FUNCTION);
                this->schedule(res.address);
                break;

                // case EmulateResult::CALLTABLE: break;

            default: break;
        }
    }
}

const Segment* Emulator::get_segment(usize idx) {
    if(m_segment && (idx >= m_segment->index && idx < m_segment->endindex))
        return m_segment;

    m_segment = state::context->index_to_segment(idx);
    return m_segment;
}

void Emulator::next() {
    if(m_pending.empty())
        return;

    RDAddress address = m_pending.front();
    m_pending.pop_front();

    this->decode(EmulateResult{address}).map([&](RDAddress nextaddress) {
        this->enqueue(nextaddress);
    });
}

bool Emulator::has_next() const {
    return state::context->processor->emulate && !m_pending.empty();
}

} // namespace redasm
