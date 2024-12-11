#pragma once

#include <deque>
#include <redasm/instruction.h>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm {

class Emulator {
public:
    [[nodiscard]] bool has_pending() const;
    void add_ref(MIndex toidx, usize type);
    void enqueue_flow(MIndex index);
    void enqueue_jump(MIndex index);
    void enqueue_call(MIndex index);
    void tick();

public:
    MIndex pc{};

private:
    std::deque<MIndex> m_qflow;
    std::deque<MIndex> m_qjump;
    std::deque<MIndex> m_qcall;
};

} // namespace redasm
