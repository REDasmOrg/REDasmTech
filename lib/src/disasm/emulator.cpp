#include "emulator.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

void Emulator::setup() {
    assume(state::context);
    assume(m_state.registers.empty());
    assume(m_state.states.empty());

    this->dslotinstr = std::make_unique<RDInstruction>();
    const Context* ctx = state::context;

    if(ctx->processorplugin && ctx->processorplugin->setup)
        ctx->processorplugin->setup(ctx->processor, api::to_c(this));
}

void Emulator::flow(RDAddress address) {
    RDSegmentNew* fromseg = state::context->program.find_segment(this->pc);
    assume(fromseg);
    assume(fromseg->perm & SP_X);

    // Avoid inter-segment flow
    if(address >= fromseg->start && address < fromseg->end) {
        memory::set_flag(fromseg, this->pc, BF_FLOW);

        if(this->ndslot) { // Set delay flow too
            assume(this->dslotinstr->delayslots > 0);

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

u64 Emulator::get_reg(int regid) const {
    auto it = m_state.registers.find(regid);
    if(it != m_state.registers.end()) return it->second;
    return {};
}

void Emulator::set_reg(int regid, u64 val) { m_state.registers[regid] = val; }

u64 Emulator::upd_reg(int regid, u64 val, u64 mask) {
    auto it = m_state.registers.find(regid);

    if(it != m_state.registers.end()) {
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

    except("State '{}' not found", s);
}

void Emulator::enqueue_flow(RDAddress address) {
    if(m_qflow.empty() || m_qflow.front() != address)
        m_qflow.push_front(address);
}

void Emulator::enqueue_jump(RDAddress address) {
    // TODO(davide): Snapshot State
    if(m_qjump.empty() || m_qjump.front() != address)
        m_qjump.push_back(address);
}

void Emulator::enqueue_call(RDAddress address) {
    // TODO(davide): Snapshot State
    if(m_qcall.empty() || m_qcall.front() != address)
        m_qcall.push_back(address);
}

u32 Emulator::tick() {
    RDAddress address;

    if(!m_qflow.empty()) {
        address = m_qflow.front();
        m_qflow.pop_front();
    }
    else if(!m_qjump.empty()) {
        address = m_qjump.front();
        m_qjump.pop_front();
    }
    else if(!m_qcall.empty()) {
        address = m_qcall.front();
        m_qcall.pop_front();
    }
    else
        return 0;

    Context* ctx = state::context;

    const RDProcessorPlugin* plugin = ctx->processorplugin;
    assume(plugin);

    RDSegmentNew* seg = ctx->program.find_segment(address);
    assume(seg);

    if(memory::has_flag(seg, address, BF_CODE))
        return memory::get_length(seg, address);

    this->pc = address;

    RDInstruction instr = {
        .address = address,
        .features = this->ndslot ? IF_DSLOT : IF_NONE,
    };

    if(plugin->decode)
        plugin->decode(ctx->processor, &instr);
    else {
        ctx->add_problem(
            address, fmt::format("decode() not implemented for processor '{}'",
                                 plugin->name));
        return 0;
    }

    if(instr.length) {
        memory::unset_n(seg, address, instr.length);
        assume(plugin->emulate);
        plugin->emulate(ctx->processor, api::to_c(this), &instr);
        memory::set_n(seg, address, instr.length, BF_CODE);

        if(instr.features & IF_JUMP) memory::set_flag(seg, address, BF_JUMP);
        if(instr.features & IF_CALL) memory::set_flag(seg, address, BF_CALL);
        if(instr.delayslots) this->execute_delayslots(seg, instr);
    }

    return instr.length;
}

void Emulator::execute_delayslots(RDSegmentNew* seg,
                                  const RDInstruction& instr) {
    *this->dslotinstr = instr;

    // Continue through delay slot(s)
    memory::set_flag(seg, this->pc, BF_DFLOW);

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

bool Emulator::has_pending_code() const {
    return state::context->processorplugin->emulate &&
           (!m_qflow.empty() || !m_qjump.empty() || !m_qcall.empty());
}

} // namespace redasm
