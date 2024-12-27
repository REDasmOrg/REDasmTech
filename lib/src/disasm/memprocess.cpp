#include "memprocess.h"
#include "../context.h"
#include "../listing.h"
#include "../memory/stringfinder.h"
#include "../state.h"
#include "../typing/base.h"
#include "function.h"

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

    if(idx > start)
        listing.hex_dump(start, idx);
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
        assume(len > 1);
        listing.fill(idx, idx + len);
        idx += len;
    }
    else {
        except("Unhandled data byte @ index {:x}, value {}", idx,
               ctx->to_hex(b.value, 8));
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

        // Ignore loops
        if(done.contains(startidx))
            continue;

        done.insert(startidx);

        RDGraphNode n = f.try_add_block(startidx);

        if(startidx == idx)
            f.graph.set_root(n);

        MIndex curridx = startidx;
        Byte b = mem->at(curridx);

        while(curridx < mem->size()) {
            // Don't overlap functions & blocks
            if((startidx != curridx) && b.has(BF_FUNCTION))
                break;

            // Break loop and connect basic blocks
            if((startidx != curridx) && b.has(BF_JUMPDST)) {
                pending.push_back(curridx);
                f.jmp(n, f.try_add_block(curridx));
                break;
            }

            bool isjump = b.has(BF_JUMP);

            if(isjump) {
                Function::BasicBlock* bb = f.get_basic_block(n);
                assume(bb);
                bb->end = curridx;

                for(Database::Ref r :
                    ctx->get_refs_from_type(curridx, CR_JUMP)) {
                    const Segment* seg = ctx->index_to_segment(r.index);

                    if(seg && seg->type & SEG_HASCODE) {
                        if(!mem->at(r.index).has(BF_CODE))
                            continue;

                        pending.push_back(r.index);
                        f.jmp_true(n, f.try_add_block(r.index));
                    }
                }

                if(b.has(BF_DFLOW)) { // Consume delay slot(s)
                    Function::BasicBlock* bb = f.get_basic_block(n);
                    assume(bb);

                    while(curridx < mem->size() &&
                          mem->at(curridx).has(BF_DFLOW)) {
                        bb->end = curridx;
                        usize len = mem->get_length(curridx);
                        assume(len > 0);
                        curridx += len;
                    }

                    if(curridx < mem->size())
                        b = mem->at(curridx);
                    else
                        break;
                }
            }

            if(b.has(BF_FLOW)) {
                usize len = mem->get_length(curridx);
                assume(len > 0);
                MIndex flow = curridx + len;

                if(isjump) {
                    pending.push_back(flow);
                    f.jmp_false(n, f.try_add_block(flow));
                    break;
                }

                Function::BasicBlock* bb = f.get_basic_block(n);
                assume(bb);
                bb->end = curridx;
                curridx = flow;
                b = mem->at(curridx);
            }
            else {
                Function::BasicBlock* bb = f.get_basic_block(n);
                assume(bb);
                bb->end = curridx;
                break;
            }
        }
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

    usize l = ctx->memory->get_length(idx);
    assume(l > 0);
    idx += l;
}

void process_refsto(Context* ctx, MIndex idx) {
    const Byte& b = ctx->memory->at(idx);
    if(!b.is_unknown())
        return;

    auto is_range_unknown = [&](MIndex ridx, usize sz) {
        return ctx->memory->range_is(ridx, sz,
                                     [](Byte b) { return b.is_unknown(); });
    };

    assume(ctx->processor);
    Database::RefList refs = ctx->get_refs_to(idx);
    RDType addrtype = ctx->types.int_from_bytes(ctx->processor->address_size);
    RDType inttype = ctx->types.int_from_bytes(ctx->processor->integer_size);

    for(const Database::Ref& r : refs) {
        if(!b.is_unknown())
            break;

        switch(r.type) {
            case DR_READ:
            case DR_WRITE: {
                stringfinder::classify(idx)
                    .map([&](const RDStringResult& x) {
                        if(is_range_unknown(idx, x.totalsize))
                            ctx->set_type(idx, x.type, ST_WEAK);
                    })
                    .or_else([&]() {
                        if(is_range_unknown(idx, ctx->processor->integer_size))
                            ctx->set_type(idx, inttype, ST_WEAK);
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
                        if(is_range_unknown(idx, ctx->processor->address_size))
                            ctx->set_type(idx, addrtype, ST_WEAK);
                    });
                break;
            }

            default: break;
        }
    }
}

} // namespace

void process_listing() {
    const Context* ctx = state::context;

    Listing l;
    FunctionList f;

    if(ctx->memory) {
        for(usize idx = 0; idx < ctx->memory->size();) {
            Byte b = ctx->memory->at(idx);

            if(b.has(BF_SEGMENT))
                l.segment(idx);

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

void process_segments(bool finalize) {
    spdlog::info("Processing segments...");

    FunctionList f;
    Context* ctx = state::context;

    if(ctx->memory) {
        for(const Segment& seg : ctx->segments) {
            for(usize idx = seg.index;
                idx < seg.endindex && idx < ctx->memory->size(); idx++) {
                Byte b = ctx->memory->at(idx);

                if(b.has(BF_FUNCTION))
                    mem::process_function_graph(ctx, f, idx);

                if(finalize) {
                    if(b.has(BF_REFSTO))
                        mem::process_refsto(ctx, idx);
                }
            }
        }
    }

    state::context->functions = std::move(f);
}

} // namespace redasm::mem
