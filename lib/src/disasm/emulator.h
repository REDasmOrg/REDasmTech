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
        std::unordered_map<int, u64> sregs;
        std::unordered_map<int, u64> regs;
        std::map<std::string, u64, std::less<>> states;
    };

    using Snapshot = std::pair<RDAddress, State>;

public:
    Emulator();
    void setup();
    bool decode_prev(RDAddress address, RDInstruction& instr);
    bool decode(RDAddress address, RDInstruction& instr);
    bool has_pending_code() const;
    tl::optional<u64> get_reg(int regid) const;
    void unset_reg(int regid);
    void set_reg(int regid, u64 val);
    tl::optional<u64> upd_reg(int regid, u64 val, u64 mask);
    u64 get_state(std::string_view s) const;
    void set_state(const std::string& s, u64 val);
    void del_state(std::string_view s);
    u64 take_state(std::string_view s);
    u64 upd_state(std::string_view s, u64 val, u64 mask);
    void add_ref(RDAddress toaddr, usize type);
    void unset_sreg(RDAddress addr, int reg);
    void set_sreg(RDAddress addr, int reg, u64 val);
    void flow(RDAddress address);
    u32 tick();

    void reset() { m_state = {}; }
    void enqueue_flow(RDAddress address) { m_flow = address; }

    void enqueue_jump(RDAddress address) {
        m_qjump.emplace_back(address, m_state);
    }

    void enqueue_call(RDAddress address) {
        m_qcall.emplace_back(address, m_state);
    };

private:
    void check_sregs();
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
