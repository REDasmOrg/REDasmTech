#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"
// #include <algorithm>

namespace redasm {

namespace {

// void link_refs(usize idx, RDAddress refaddress) {
//     AddressDetail& d = state::context->database.get_detail(idx);
//
//     state::context->address_to_index(refaddress).map([&](usize x) {
//         auto it = std::lower_bound(d.refs.begin(), d.refs.end(), x);
//         if(it == d.refs.end() || (*it != x))
//             d.refs.insert(it, x);
//     });
// }

// void link_next(usize idx, RDAddress nextaddress) {
//     AddressDetail& d = state::context->database.get_detail(idx);
//
//     state::context->address_to_index(nextaddress).map([&](usize x) {
//         auto it = std::lower_bound(d.next.begin(), d.next.end(), x);
//         if(it == d.next.end() || (*it != x))
//             d.next.insert(it, x);
//     });
// }

} // namespace

void Emulator::enqueue(RDAddress address) { m_pending.push_front(address); }
void Emulator::schedule(RDAddress address) { m_pending.push_back(address); }

tl::optional<RDAddress> Emulator::decode(RDAddress address) {
    if(!state::context->is_address(address))
        return tl::nullopt;

    auto idx = state::context->address_to_index(address);
    assume(idx.has_value());

    if(const Segment* s = this->get_segment(*idx);
       !s || !(s->type & SEGMENTTYPE_HASCODE))
        return tl::nullopt;

    Memory* m = state::context->memory.get();

    if(Byte b = m->at(*idx); !b.has_byte() || !b.is_unknown())
        return tl::nullopt;

    const RDProcessor* p = state::context->processor;
    assume(p);

    if(!p->emulate)
        return tl::nullopt;

    RDEmulateResult r = {
        address,
        0,
        true,
    };

    usize size = p->emulate(p, &r);

    if(size) {
        m->set_code(*idx, size);

        if(r.delayslot) {
            // TODO(davide): Handle delay slots
        }

        if(r.canflow) {
            RDAddress nextaddress = address + size;
            // link_next(*idx, nextaddress);
            return nextaddress;
        }
    }

    return tl::nullopt;
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

    this->decode(address).map(
        [&](RDAddress nextaddress) { this->enqueue(nextaddress); });
}

bool Emulator::has_next() const {
    return state::context->processor->emulate && !m_pending.empty();
}

} // namespace redasm
