#pragma once

#include "../segment.h"
#include <deque>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm {

class Emulator {
public:
    bool enqueue(MIndex idx);
    bool schedule(MIndex idx);
    void add_ref(MIndex idx, usize type);
    void set_type(MIndex idx, std::string_view tname);
    void next();
    tl::optional<RDAddress> get_next_address() const;
    [[nodiscard]] bool has_next() const;

private:
    void decode(MIndex address);
    const Segment* get_segment(MIndex idx);

private:
    std::deque<usize> m_pending;
    usize m_currindex{};
    const Segment* m_segment{nullptr};
};

} // namespace redasm
