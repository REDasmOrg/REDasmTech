#include "memprocess.h"
#include "../context.h"
#include "../listing.h"
#include "../memory/stringfinder.h"
#include "../state.h"
#include "../typing/base.h"
#include "function.h"
#include <unordered_set>

namespace redasm::mem {

namespace {

void process_listing_array(const Context* ctx, Listing& listing, MIndex& idx,
                           RDType t);

template<typename Function>
void process_hexdump(const Context* ctx, Listing& listing, MIndex& idx,
                     Function f) {
    usize l = 0, start = idx;

    for(; idx < ctx->memory->size() && f(ctx->memory->at(idx)); l++, idx++) {
        if(idx != start && ctx->memory->at(idx).has(BF_SEGMENT))
            break; // Split inter-segment unknowns

        if(l && !(l % 0x10)) {
            listing.hex_dump(start, idx);
            start = idx;
        }
    }

    if(idx > start) listing.hex_dump(start, idx);
}

void process_listing_unknown(const Context* ctx, Listing& listing,
                             MIndex& idx) {
    mem::process_hexdump(ctx, listing, idx,
                         [](Byte b) { return b.is_unknown(); });
}

LIndex process_listing_type(const Context* ctx, Listing& listing, MIndex& idx,
                            RDType t) {
    LIndex listingidx = listing.size();
    listing.type(idx, t);

    const typing::TypeDef* td = ctx->types.get_typedef(t);
    assume(td);

    if(td->is_struct()) { // Struct creates a new scope
        listing.push_type(t);
        listing.push_indent();

        for(usize j = 0; j < td->dict.size(); j++) {
            const auto& [fieldtype, _] = td->dict[j];

            const typing::TypeDef* ftd = ctx->types.get_typedef(fieldtype);
            assume(ftd);

            listing.push_fieldindex(j);

            if(fieldtype.n > 0) {
                mem::process_listing_array(ctx, listing, idx,
                                           ftd->to_type(fieldtype.n));
            }
            else
                mem::process_listing_type(ctx, listing, idx, ftd->to_type());

            listing.pop_fieldindex();
        }

        listing.pop_indent();
        listing.pop_type();
    }
    else {
        switch(td->get_id()) {
            case typing::ids::CHAR:
            case typing::ids::WCHAR:
            case typing::ids::I8:
            case typing::ids::U8:
            case typing::ids::I16:
            case typing::ids::U16:
            case typing::ids::I32:
            case typing::ids::U32:
            case typing::ids::I64:
            case typing::ids::U64: idx += td->size; break;

            case typing::ids::WSTR:
            case typing::ids::STR: idx += ctx->memory->get_length(idx); break;

            default: unreachable;
        }
    }

    return listingidx;
}

void process_listing_array(const Context* ctx, Listing& listing, MIndex& idx,
                           RDType t) {
    assume(t.n > 0);

    listing.type(idx, t);
    listing.push_indent();

    const typing::TypeDef* td = ctx->types.get_typedef(t);
    assume(td);

    switch(td->get_id()) {
        // Array of chars are handled differently
        case typing::ids::CHAR:
        case typing::ids::WCHAR: idx += td->size * t.n; break;

        default: {
            for(usize i = 0; i < std::max<usize>(t.n, 1); i++) {
                LIndex lidx =
                    mem::process_listing_type(ctx, listing, idx, td->to_type());
                listing[lidx].array_index = i;
            }
            break;
        }
    }

    listing.pop_indent();
}

void process_listing_data(const Context* ctx, Listing& listing, MIndex& idx) {
    Byte b = ctx->memory->at(idx);

    if(b.has(BF_TYPE)) {
        auto type = ctx->get_type(idx);
        assume(type);

        if(type->n > 0)
            mem::process_listing_array(ctx, listing, idx, *type);
        else
            mem::process_listing_type(ctx, listing, idx, *type);
    }
    else if(b.has(BF_FILL)) {
        usize len = ctx->memory->get_length(idx);
        if(len <= 1) except("Invalid length @ {:x} (FILL)", idx);
        listing.fill(idx, idx + len);
        idx += len;
    }
    else {
        except("Unhandled data byte @ index {:x}, value {}", idx,
               ctx->to_hex(b.value, 8));
    }
}

void process_unknown_data(Context* ctx, MIndex& idx) {
    auto& mem = ctx->memory;
    usize startidx = idx++;
    Byte startb = mem->at(startidx);
    const Segment* startseg = ctx->index_to_segment(startidx);

    while(idx < mem->size()) {
        // Don't merge inter-segment bytes
        const Segment* seg = ctx->index_to_segment(idx);
        if(startseg != seg) break;

        Byte b = mem->at(idx);

        if(!b.is_unknown() || b.has_common() ||
           (b.has_byte() != startb.has_byte()) ||
           (b.has_byte() && (b.byte() != startb.byte())))
            break;

        idx++;
    }

    usize len = idx - startidx;

    if(len > 1 &&
       len > static_cast<usize>(ctx->processorplugin->integer_size)) {
        mem->set_n(startidx, len, BF_DATA);
        mem->set(startidx, BF_FILL);
    }
}

void process_function_graph(const Context* ctx, FunctionList& functions,
                            MIndex idx) {
    auto address = ctx->index_to_address(idx);
    assume(address.has_value());

    spdlog::info("Creating function graph @ {}", ctx->to_hex(*address));

    const auto& mem = ctx->memory;
    Function& f = functions.emplace_back(idx);
    std::unordered_set<MIndex> done;
    std::deque<MIndex> pending;
    pending.push_back(idx);

    while(!pending.empty()) {
        MIndex startidx = pending.front();
        pending.pop_front();

        if(done.contains(startidx)) // Ignore loops
            continue;
        done.insert(startidx);

        RDGraphNode n = f.try_add_block(startidx);
        if(startidx == idx) f.graph.set_root(n);

        MIndex endidx = startidx;

        // Find basic block end
        for(MIndex curridx = startidx; curridx < mem->size();) {
            Byte b = mem->at(curridx);
            if(!b.is_code()) break;

            if(curridx != startidx) {
                if(b.has(BF_FUNCTION)) break;

                if(b.has(BF_JUMPDST)) {
                    pending.push_back(curridx);
                    f.jmp_true(n, f.try_add_block(curridx));
                    break;
                }
            }

            if(b.has(BF_JUMP)) {
                for(Database::Ref r :
                    ctx->get_refs_from_type(curridx, CR_JUMP)) {
                    const Segment* seg = ctx->index_to_segment(r.index);

                    if(seg && seg->type & SEG_HASCODE) {
                        if(!mem->at(r.index).has(BF_CODE)) continue;

                        pending.push_back(r.index);
                        f.jmp_true(n, f.try_add_block(r.index));
                    }
                }

                // Consume delay slot(s), if any
                while(curridx < mem->size() && b.has(BF_DFLOW)) {
                    usize len = mem->get_length(curridx);
                    if(!len) except("Invalid length @ {:x} (DS)", curridx);
                    curridx += len;
                    b = mem->at(curridx);
                }

                endidx = curridx;

                // Conditional Jump
                if(curridx < mem->size() && b.has(BF_FLOW)) {
                    usize len = mem->get_length(curridx);
                    if(!len) except("Invalid length @ {:x} (CJ)", curridx);
                    curridx += len;

                    if(curridx < mem->size()) {
                        pending.push_back(curridx);
                        f.jmp_false(n, f.try_add_block(curridx));
                    }
                }

                break;
            }

            endidx = curridx;
            usize len = mem->get_length(curridx);
            if(!len) except("Invalid length @ {:x} (END)", curridx);
            curridx += len;

            if(!b.has(BF_FLOW)) break;
        }

        Function::BasicBlock* bb = f.get_basic_block(n);
        assume(bb);
        bb->end = std::min<usize>(endidx, mem->size() - 1);
    }
}

void process_listing_code(const Context* ctx, Listing& listing,
                          FunctionList& functions, MIndex& idx) {
    Byte b = ctx->memory->at(idx);
    assume(b.has(BF_CODE));

    if(b.has(BF_FUNCTION)) {
        listing.pop_indent(2);
        listing.function(idx);
        listing.push_indent(2);

        const Segment* s = listing.current_segment();
        assume(s);
        assume(s->type & SEG_HASCODE);
        mem::process_function_graph(ctx, functions, idx);
    }
    else if(b.has(BF_REFSTO)) {
        listing.pop_indent();
        listing.label(idx);
        listing.push_indent();
    }

    listing.instruction(idx);

    usize len = ctx->memory->get_length(idx);
    if(!len) except("Invalid length @ {:x} (CODE)", idx);
    idx += len;
}

void process_refsto(Context* ctx, MIndex idx) {
    const Byte& b = ctx->memory->at(idx);
    if(!b.is_unknown()) return;

    auto is_range_unknown = [&](MIndex ridx, usize sz) {
        return ctx->memory->range_is(ridx, sz,
                                     [](Byte b) { return b.is_unknown(); });
    };

    assume(ctx->processorplugin);
    Database::RefList refs = ctx->get_refs_to(idx);
    auto addrtype =
        ctx->types.int_from_bytes(ctx->processorplugin->address_size);
    assume(addrtype.has_value());
    auto inttype =
        ctx->types.int_from_bytes(ctx->processorplugin->integer_size);
    assume(inttype.has_value());

    for(const Database::Ref& r : refs) {
        if(!b.is_unknown()) break;

        switch(r.type) {
            case DR_READ:
            case DR_WRITE: {
                stringfinder::classify(idx)
                    .map([&](const RDStringResult& x) {
                        if(is_range_unknown(idx, x.totalsize))
                            ctx->set_type(idx, x.type, ST_WEAK);
                    })
                    .or_else([&]() {
                        if(is_range_unknown(idx,
                                            ctx->processorplugin->integer_size))
                            ctx->set_type(idx, *inttype, ST_WEAK);
                    });
                break;
            }

            case DR_ADDRESS: {
                stringfinder::classify(idx)
                    .map([&](const RDStringResult& x) {
                        if(is_range_unknown(idx, x.totalsize))
                            ctx->set_type(idx, x.type, ST_WEAK);
                    })
                    .or_else([&]() {
                        if(is_range_unknown(idx,
                                            ctx->processorplugin->address_size))
                            ctx->set_type(idx, *addrtype, ST_WEAK);
                    });
                break;
            }

            default: break;
        }
    }
}

} // namespace

void merge_code(Emulator* e) {
    const Context* ctx = state::context;
    const auto& mem = ctx->memory;

    for(const Segment& seg : ctx->segments) {
        if(!(seg.type & SEG_HASCODE) || seg.offset == seg.endoffset) continue;

        usize idx = seg.index;

        while(idx < seg.endindex && idx < mem->size()) {
            Byte b = mem->at(idx);

            if(b.has_byte() && b.is_unknown()) {
                e->enqueue_flow(idx++);

                // Move after the unknown range
                while(idx < seg.endindex && idx < mem->size() &&
                      mem->at(idx).is_unknown())
                    idx++;
            }
            else
                idx++;
        }
    }
}

void process_memory() {
    Context* ctx = state::context;
    auto& mem = ctx->memory;
    FunctionList f;

    for(usize idx = 0; idx < mem->size();) {
        Byte b = mem->at(idx);

        if(b.has(BF_FUNCTION))
            mem::process_function_graph(ctx, f, idx++);
        else if(b.has(BF_REFSTO))
            mem::process_refsto(ctx, idx++);
        else if(b.is_unknown() && !b.has(BF_REFSFROM | BF_REFSTO))
            mem::process_unknown_data(ctx, idx);
        else
            idx++;
    }

    state::context->functions = std::move(f);
}

void process_listing() {
    const Context* ctx = state::context;

    Listing l;
    FunctionList f;

    if(ctx->memory) {
        for(usize idx = 0; idx < ctx->memory->size();) {
            Byte b = ctx->memory->at(idx);
            if(b.has(BF_SEGMENT)) l.segment(idx);

            if(b.is_unknown()) {
                mem::process_listing_unknown(ctx, l, idx);
                continue;
            }

            if(b.is_data()) {
                mem::process_listing_data(ctx, l, idx);
                continue;
            }

            if(b.is_code()) {
                l.push_indent(4);
                mem::process_listing_code(ctx, l, f, idx);
                l.pop_indent(4);
                continue;
            }

            unreachable;
        }
    }

    spdlog::info("Listing completed ({} items)", l.size());
    state::context->functions = std::move(f);
    state::context->listing = std::move(l);
}

} // namespace redasm::mem
