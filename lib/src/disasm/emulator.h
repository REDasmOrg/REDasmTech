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
    void tick();

public:
    std::deque<MIndex> qflow;
    std::deque<MIndex> qjump;
    std::deque<MIndex> qcall;
    MIndex pc{};
};

} // namespace redasm
