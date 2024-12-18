#pragma once

#include "../typing/typing.h"
#include <deque>
#include <redasm/instruction.h>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <tl/optional.hpp>
#include <unordered_map>

namespace redasm {

class Emulator {
public:
    // ~Emulator();
    void activate();
    [[nodiscard]] bool has_pending_types() const;
    [[nodiscard]] bool has_pending_code() const;
    bool set_typename(MIndex idx, typing::FullTypeName tname);
    bool set_type(MIndex idx, RDType type);
    void add_ref(MIndex toidx, usize type);
    void enqueue_flow(MIndex index);
    void enqueue_jump(MIndex index);
    void enqueue_call(MIndex index);
    void tick_type();
    void tick();

private:
    void flow(MIndex idx);

public:
    MIndex current{};

private:
    // void* m_state{nullptr};
    std::unordered_map<MIndex, std::pair<RDType, bool>> m_ptypes;
    std::deque<MIndex> m_qflow;
    std::deque<MIndex> m_qjump;
    std::deque<MIndex> m_qcall;
};

} // namespace redasm
