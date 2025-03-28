#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../memory/mbyte.h"
#include "../memory/memory.h"
#include "../state.h"

namespace redasm {

Emulator::Emulator() {
    ct_assume(state::context);
    ct_assume(m_state.regs.empty());
    ct_assume(m_state.states.empty());

    this->dslotinstr = std::make_unique<RDInstruction>();
}

void Emulator::setup() {
    const Context* ctx = state::context;
    ct_assume(ctx->processorplugin);
    if(ctx->processorplugin->setup)
        ctx->processorplugin->setup(ctx->processor, api::to_c(this));
}

void Emulator::flow(RDAddress address) {
    RDSegment* fromseg = state::context->program.find_segment(this->pc);
    ct_assume(fromseg);
    ct_assume(fromseg->perm & SP_X);

    // Avoid inter-segment flow
    if(address >= fromseg->start && address < fromseg->end) {
        memory::set_flag(fromseg, this->pc, BF_FLOW);

        if(this->ndslot) { // Set delay flow too
            ct_assume(this->dslotinstr->delayslots > 0);

            // D-Flow until last delay slot
            if(this->ndslot < this->dslotinstr->delayslots)
                memory::set_flag(fromseg, this->pc, BF_FLOW);
        }

        this->enqueue_flow(address);
    }
}

void Emulator::add_ref(RDAddress toaddr, usize type) { // NOLINT
    state::context->add_ref(this->pc, toaddr, type);
}

void Emulator::add_regchange(RDAddress addr, int reg, u64 val) {
    // state::context->set_sreg(addr, reg, val, this->pc);
}

u64 Emulator::get_reg(int regid) const {
    auto it = m_state.regs.find(regid);
    if(it != m_state.regs.end()) return it->second;
    return {};
}

void Emulator::set_reg(int regid, u64 val) { m_state.regs[regid] = val; }

u64 Emulator::upd_reg(int regid, u64 val, u64 mask) {
    auto it = m_state.regs.find(regid);

    if(it != m_state.regs.end()) {
        it->second = (it->second & ~mask) | (val & mask);
        return it->second;
    }

    val &= mask;
    this->set_reg(regid, val);
    return val;
}

u64 Emulator::get_state(std::string_view s) const {
    auto it = m_state.states.find(s);
    if(it != m_state.states.end()) return it->second;
    return {};
}

void Emulator::set_state(const std::string& s, u64 val) {
    m_state.states[s] = val;
}

void Emulator::del_state(std::string_view s) {
    auto it = m_state.states.find(s);
    if(it != m_state.states.end()) m_state.states.erase(it);
}

u64 Emulator::take_state(std::string_view s) {
    auto it = m_state.states.find(s);

    if(it != m_state.states.end()) {
        u64 val = it->second;
        m_state.states.erase(it);
        return val;
    }

    return {};
}

u64 Emulator::upd_state(std::string_view s, u64 val, u64 mask) {
    auto it = m_state.states.find(s);

    if(it != m_state.states.end()) {
        it->second = (it->second & ~mask) | (val & mask);
        return it->second;
    }

    ct_exceptf("State '%.*s' not found", static_cast<int>(s.size()), s.data());
}

u32 Emulator::tick() {
    if(m_flow) {
        this->pc = m_flow.value();
        m_flow.reset();
    }
    else if(!m_qjump.empty()) {
        const Snapshot& s = m_qjump.front();
        this->pc = s.first;
        m_state = s.second;
        m_qjump.pop_front();
    }
    else if(!m_qcall.empty()) {
        const Snapshot& s = m_qcall.front();
        this->pc = s.first;
        m_state = s.second;
        m_qcall.pop_front();
    }
    else
        return 0;

    Context* ctx = state::context;

    this->segment = ctx->program.find_segment(this->pc);
    ct_assume(this->segment);

    if(memory::has_flag(this->segment, this->pc, BF_CODE))
        return memory::get_length(this->segment, this->pc);

    RDInstruction instr = {
        .features = this->ndslot ? IF_DSLOT : IF_NONE,
    };

    if(!this->decode(this->pc, instr)) return 0;

    if(instr.length) {
        const RDProcessorPlugin* plugin = ctx->processorplugin;
        ct_assume(plugin);

        memory::unset_n(this->segment, this->pc, instr.length);
        ct_assume(plugin->emulate);
        plugin->emulate(ctx->processor, api::to_c(this), &instr);
        memory::set_n(this->segment, this->pc, instr.length, BF_CODE);

        if(instr.features & IF_JUMP)
            memory::set_flag(this->segment, this->pc, BF_JUMP);
        if(instr.features & IF_CALL)
            memory::set_flag(this->segment, this->pc, BF_CALL);

        if(instr.delayslots) this->execute_delayslots(instr);
    }

    return instr.length;
}

void Emulator::execute_delayslots(const RDInstruction& instr) {
    *this->dslotinstr = instr;

    // Continue through delay slot(s)
    memory::set_flag(this->segment, this->pc, BF_DFLOW);

    for(this->ndslot = 1; this->ndslot <= instr.delayslots; this->ndslot++) {
        u32 len = this->tick();

        if(!len) {
            state::context->add_problem(
                this->pc,
                fmt::format("Cannot decode delay slot #{}", this->ndslot));
            break;
        }
    }

    this->ndslot = 0;
}

bool Emulator::decode_prev(RDAddress address, RDInstruction& instr) {
    RDSegment* seg = state::context->program.find_segment(address);
    if(!seg || seg->start == address) return false;

    auto mb = memory::get_mbyte(seg, address - 1);
    if(!mb || !mbyte::has_byte(*mb) || !mbyte::is_code(*mb)) return false;

    if(auto prevaddr = memory::get_prev(seg, address); prevaddr)
        return this->decode(*prevaddr, instr);

    return false;
}

bool Emulator::decode(RDAddress address, RDInstruction& instr) {
    const RDProcessorPlugin* plugin = state::context->processorplugin;
    ct_assume(plugin);

    // this->check_sregs();
    instr.address = address;

    if(plugin->decode)
        plugin->decode(state::context->processor, &instr);
    else {
        state::context->add_problem(
            address, fmt::format("decode() not implemented for processor '{}'",
                                 plugin->name));
        return false;
    }

    return instr.length > 0;
}

void Emulator::check_sregs() {
    Database::SRegChanges rc = state::context->get_sregs_from_addr(this->pc);

    for(const Database::SegmentReg& sreg : rc)
        m_state.sregs[sreg.reg] = sreg.value;
}

bool Emulator::has_pending_code() const {
    return state::context->processorplugin->emulate &&
           (m_flow.has_value() || !m_qjump.empty() || !m_qcall.empty());
}

} // namespace redasm
