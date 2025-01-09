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

void Emulator::flow(MIndex index) {
    const Segment* fromseg = state::context->index_to_segment(this->pc);
    assume(fromseg);
    assume(fromseg->type & SEG_HASCODE);

    // Avoid inter-segment flow
    if(index >= fromseg->index && index < fromseg->endindex) {
        state::context->memory->set(this->pc, BF_FLOW);

        if(this->ndslot) { // Set delay flow too
            assume(this->dslotinstr->delayslots > 0);

            // D-Flow until last delay slot
            if(this->ndslot < this->dslotinstr->delayslots)
                state::context->memory->set(this->pc, BF_DFLOW);
        }

        this->enqueue_flow(index);
    }
}

void Emulator::add_ref(MIndex toidx, usize type) { // NOLINT
    state::context->add_ref(this->pc, toidx, type);
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

void Emulator::enqueue_flow(MIndex index) {
    if(m_qflow.empty() || m_qflow.front() != index) m_qflow.push_front(index);
}

void Emulator::enqueue_jump(MIndex index) {
    // TODO(davide): Snapshot State
    if(m_qjump.empty() || m_qjump.front() != index) m_qjump.push_back(index);
}

void Emulator::enqueue_call(MIndex index) {
    // TODO(davide): Snapshot State
    if(m_qcall.empty() || m_qcall.front() != index) m_qcall.push_back(index);
}

u32 Emulator::tick() {
    MIndex idx;

    if(!m_qflow.empty()) {
        idx = m_qflow.front();
        m_qflow.pop_front();
    }
    else if(!m_qjump.empty()) {
        idx = m_qjump.front();
        m_qjump.pop_front();
    }
    else if(!m_qcall.empty()) {
        idx = m_qcall.front();
        m_qcall.pop_front();
    }
    else
        return 0;

    Context* ctx = state::context;
    auto& mem = ctx->memory;

    const RDProcessorPlugin* plugin = ctx->processorplugin;
    assume(plugin);

    if(ctx->memory->at(idx).is_code()) return ctx->memory->get_length(idx);

    this->pc = idx;

    auto address = ctx->index_to_address(idx);
    assume(address.has_value());

    RDInstruction instr = {
        .address = *address,
        .features = this->ndslot ? IF_DSLOT : IF_NONE,
    };

    if(plugin->decode)
        plugin->decode(ctx->processor, &instr);
    else {
        ctx->add_problem(
            idx, fmt::format("decode() not implemented for processor '{}'",
                             plugin->name));
        return 0;
    }

    if(instr.length) {
        mem->unset_n(idx, instr.length);
        assume(plugin->emulate);
        plugin->emulate(ctx->processor, api::to_c(this), &instr);
        mem->set_n(idx, instr.length, BF_CODE);

        if(instr.features & IF_JUMP) mem->set(idx, BF_JUMP);
        if(instr.features & IF_CALL) mem->set(idx, BF_CALL);
        if(instr.delayslots) this->execute_delayslots(instr);
    }

    return instr.length;
}

void Emulator::execute_delayslots(const RDInstruction& instr) {
    *this->dslotinstr = instr;

    Context* ctx = state::context;
    auto& mem = ctx->memory;

    // Continue through delay slot(s)
    mem->set(this->pc, BF_DFLOW);

    for(this->ndslot = 1; this->ndslot <= instr.delayslots; this->ndslot++) {
        u32 len = this->tick();

        if(!len) {
            ctx->add_problem(
                this->pc,
                fmt::format("Cannot decode delay slot #{}", this->ndslot));
            break;
        }
    }

    this->ndslot = 0;
}

bool Emulator::has_pending_code() const {
    return state::context->memory && state::context->processorplugin->emulate &&
           (!m_qflow.empty() || !m_qjump.empty() || !m_qcall.empty());
}

} // namespace redasm
