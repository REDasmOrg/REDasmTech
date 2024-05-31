#pragma once

#include "../segment.h"
#include <deque>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm {

class Emulator {
public:
    void enqueue(RDAddress address);
    void schedule(RDAddress address);
    void next();
    [[nodiscard]] bool has_next() const;

private:
    tl::optional<RDAddress> decode(RDAddress address);
    const Segment* get_segment(usize idx);

private:
    std::deque<RDAddress> m_pending;
    const Segment* m_segment{nullptr};
};

} // namespace redasm
