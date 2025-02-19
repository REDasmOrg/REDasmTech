#pragma once

#include <deque>
#include <map>
#include <memory>
#include <redasm/instruction.h>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

namespace redasm {

class Emulator {
    struct State {
        std::unordered_map<int, u64> registers;
        std::map<std::string, u64, std::less<>> states;
    };

    using Snapshot = std::pair<RDAddress, State>;

public:
    Emulator();
    bool has_pending_code() const;
    u64 get_reg(int regid) const;
    void set_reg(int regid, u64 val);
    u64 upd_reg(int regid, u64 val, u64 mask);
    u64 get_state(std::string_view s) const;
    void set_state(const std::string& s, u64 val);
    void del_state(std::string_view s);
    u64 take_state(std::string_view s);
    u64 upd_state(std::string_view s, u64 val, u64 mask);
    void flow(RDAddress address);
    void add_ref(RDAddress toaddr, usize type);
    void enqueue_flow(RDAddress address);
    void enqueue_jump(RDAddress address);
    void enqueue_call(RDAddress address);
    u32 tick();

private:
    void execute_delayslots(const RDInstruction& instr);

public:
    RDAddress pc{};
    RDSegment* segment{nullptr};
    std::unique_ptr<RDInstruction> dslotinstr;
    u32 ndslot{0}; // =0 no delay slot

private:
    State m_state;
    tl::optional<RDAddress> m_flow;
    std::deque<Snapshot> m_qjump;
    std::deque<Snapshot> m_qcall;
};

} // namespace redasm
