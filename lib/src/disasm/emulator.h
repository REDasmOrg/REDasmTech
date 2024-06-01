#pragma once

#include "../segment.h"
#include <deque>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm {

class Emulator {
public:
    void enqueue(usize idx);
    void schedule(usize idx);
    void check_location(RDAddress address);
    void add_coderef(usize idx, usize cr);
    void add_dataref(usize idx, usize dr);
    void next();
    [[nodiscard]] bool has_next() const;

private:
    tl::optional<RDAddress> decode(RDAddress address);
    const Segment* get_segment(usize idx);

private:
    std::deque<usize> m_pending;
    usize m_currindex{};
    const Segment* m_segment{nullptr};
};

} // namespace redasm
