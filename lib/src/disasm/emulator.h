#pragma once

#include "../typing/typing.h"
#include <deque>
#include <redasm/instruction.h>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm {

class Emulator {
public:
    // ~Emulator();
    void activate();
    [[nodiscard]] bool has_pending_code() const;
    void add_ref(MIndex toidx, usize type);
    void enqueue_flow(MIndex index);
    void enqueue_jump(MIndex index);
    void enqueue_call(MIndex index);
    void tick();

private:
    void flow(MIndex idx);

public:
    MIndex current{};

private:
    // void* m_state{nullptr};
    std::deque<MIndex> m_qflow;
    std::deque<MIndex> m_qjump;
    std::deque<MIndex> m_qcall;
};

} // namespace redasm
