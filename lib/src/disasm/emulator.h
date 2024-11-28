#pragma once

#include "../database/database.h"
#include "../segment.h"
#include <deque>
#include <redasm/instruction.h>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm {

class Emulator {
    struct QueueItem {
        tl::optional<AddressDetail::Ref> from;
        MIndex index;
    };

public:
    [[nodiscard]] bool has_pending() const;
    void add_ref(MIndex fromidx, MIndex toidx, usize type);
    void tick();

    void add_ref(MIndex toidx, usize type) {
        this->add_ref(this->pc, toidx, type);
    }

private:
    bool check_qitem(const QueueItem& item, const std::string& errmsg) const;
    const Segment* get_segment(MIndex idx) const;

public:
    std::deque<QueueItem> qflow;
    std::deque<QueueItem> qjump;
    std::deque<QueueItem> qcall;
    MIndex pc{};

private:
    std::deque<MIndex> m_pending; // Obsolete?
    mutable const Segment* m_currsegment{nullptr};
};

} // namespace redasm
