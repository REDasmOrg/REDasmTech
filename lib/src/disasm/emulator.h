#pragma once

#include <deque>
#include <map>
#include <redasm/instruction.h>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

namespace redasm {

struct EmulatorState {
    std::unordered_map<int, u64> registers;
    std::map<std::string, u64, std::less<>> states;
};

class Emulator {
public:
    void setup();
    bool has_pending_code() const;
    u64 get_reg(int regid) const;
    void set_reg(int regid, u64 val);
    u64 upd_reg(int regid, u64 val, u64 mask);
    u64 get_state(std::string_view s) const;
    void set_state(const std::string& s, u64 val);
    void del_state(std::string_view s);
    u64 take_state(std::string_view s);
    u64 upd_state(std::string_view s, u64 val, u64 mask);
    void flow(MIndex idx);
    void add_ref(MIndex toidx, usize type);
    void enqueue_flow(MIndex index);
    void enqueue_jump(MIndex index);
    void enqueue_call(MIndex index);
    void tick();

public:
    MIndex current{};

private:
    EmulatorState m_state;
    std::deque<MIndex> m_qflow;
    std::deque<MIndex> m_qjump;
    std::deque<MIndex> m_qcall;
};

} // namespace redasm
