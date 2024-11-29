#pragma once

#include "../segment.h"
#include <deque>
#include <redasm/instruction.h>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm {

class Emulator {
public:
    [[nodiscard]] bool has_pending() const;
    void add_ref(MIndex fromidx, MIndex toidx, usize type);
    void tick();

    void add_ref(MIndex toidx, usize type) {
        this->add_ref(this->pc, toidx, type);
    }

private:
    const Segment* get_segment(MIndex idx) const;

public:
    std::deque<MIndex> qflow;
    std::deque<MIndex> qjump;
    std::deque<MIndex> qcall;
    MIndex pc{};

private:
    std::deque<MIndex> m_pending; // Obsolete?
    mutable const Segment* m_currsegment{nullptr};
};

} // namespace redasm
