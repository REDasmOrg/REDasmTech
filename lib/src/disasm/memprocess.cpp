#include "memprocess.h"
#include "../context.h"
#include "../error.h"
#include "../listing.h"
#include "../memory/mbyte.h"
#include "../memory/memory.h"
#include "../memory/stringfinder.h"
#include "../state.h"
#include "../typing/base.h"
#include "../utils/pattern.h"
#include "../utils/utils.h"
#include "function.h"
#include <unordered_set>

namespace redasm::memprocess {

namespace {

void process_listing_array(const Context* ctx, Listing& l, RDAddress& address,
                           RDType t);

template<typename Function>
void process_hexdump(Listing& l, RDAddress& address, Function f) {
    const RDSegment* seg = l.current_segment();
    assume(seg);

    RDAddress start = address;
    usize len = 0;

    for(; address < seg->end && f(seg, address); len++, address++) {
        if(len && !(len % 0x10)) {
            l.hex_dump(start, address);
            start = address;
        }
    }

    if(address > start) l.hex_dump(start, address);
}

void process_regions() {
    Context* ctx = state::context;
    assume(ctx);
    map_clear(ctx->program.regions);

    Database::RegList regs = ctx->get_changed_regs();

    for(int r : regs) {
        Database::RegChanges changes = ctx->get_regchanges_from_reg(r);

        std::ranges::sort(changes, [](const Database::RegChange& a,
                                      const Database::RegChange& b) {
            return a.address < b.address;
        });

        Vect(RDRegion) regions = vect_create(RDRegion);

        for(usize i = 0; i < changes.size(); i++) {
            const Database::RegChange& rc = changes[i];
            RDAddress start = rc.address, end;

            if(i + 1 < changes.size())
                end = changes[i + 1].address;
            else
                end = static_cast<RDAddress>(-1);

            vect_add(RDRegion, regions,
                     {
                         .start = start,
                         .end = end,
                         .value = rc.value,
                     });
        }

        map_set(RDProgramRegion, ctx->program.regions, r, regions);
    }
}

void process_listing_unknown(Listing& l, RDAddress& address) {
    memprocess::process_hexdump(l, address,
                                [](const RDSegment* seg, RDAddress addr) {
                                    return memory::is_unknown(seg, addr);
                                });
}

LIndex process_listing_type(const Context* ctx, Listing& l, RDAddress& address,
                            RDType t) {
    LIndex lidx = l.size();
    l.type(address, t);

    const typing::TypeDef* td = ctx->types.get_typedef(t);
    assume(td);

    if(td->is_struct()) { // Struct creates a new scope
        l.push_type(t);
        l.push_indent();

        for(usize j = 0; j < td->dict.size(); j++) {
            const auto& [fieldtype, _] = td->dict[j];

            const typing::TypeDef* ftd = ctx->types.get_typedef(fieldtype);
            assume(ftd);

            l.push_fieldindex(j);

            if(fieldtype.n > 0) {
                memprocess::process_listing_array(ctx, l, address,
                                                  ftd->to_type(fieldtype.n));
            }
            else
                memprocess::process_listing_type(ctx, l, address,
                                                 ftd->to_type());

            l.pop_fieldindex();
        }

        l.pop_indent();
        l.pop_type();
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
            case typing::ids::U64: address += td->size; break;

            case typing::ids::WSTR:
            case typing::ids::STR: {
                const RDSegment* seg = l.current_segment();
                assume(seg);
                address += memory::get_length(seg, address);
                break;
            }

            default: unreachable;
        }
    }

    return lidx;
}

void process_listing_array(const Context* ctx, Listing& l, RDAddress& address,
                           RDType t) {
    assume(t.n > 0);

    l.type(address, t);
    l.push_indent();

    const typing::TypeDef* td = ctx->types.get_typedef(t);
    assume(td);

    switch(td->get_id()) {
        // Array of chars are handled differently
        case typing::ids::CHAR:
        case typing::ids::WCHAR: address += td->size * t.n; break;

        default: {
            for(usize i = 0; i < std::max<usize>(t.n, 1); i++) {
                LIndex lidx = memprocess::process_listing_type(ctx, l, address,
                                                               td->to_type());
                l[lidx].array_index = i;
            }
            break;
        }
    }

    l.pop_indent();
}

void process_listing_data(const Context* ctx, Listing& l, RDAddress& address) {
    const RDSegment* seg = l.current_segment();
    assume(seg);

    if(memory::has_flag(seg, address, BF_TYPE)) {
        auto type = ctx->get_type(address);
        assume(type);

        if(type->n > 0)
            memprocess::process_listing_array(ctx, l, address, *type);
        else
            memprocess::process_listing_type(ctx, l, address, *type);
    }
    else if(memory::has_flag(seg, address, BF_FILL)) {
        usize len = memory::get_length(seg, address);
        if(len <= 1) except("Invalid length @ {:x} (FILL)", address);
        l.fill(address, address + len);
        address += len;
    }
    else {
        auto mb = memory::get_mbyte(seg, address);
        assume(mb);
        except("Unhandled data byte @ {:x}, value {}", address,
               utils::to_hex(*mb, 32));
    }
}

void process_unknown_data(Context* ctx, RDSegment* seg, RDAddress& address) {
    RDAddress startaddr = address++;

    while(address < seg->end) {
        if(!memory::is_unknown(seg, address) ||
           memory::has_common(seg, address) ||
           (memory::has_byte(seg, address) !=
                memory::has_byte(seg, startaddr) ||
            (memory::has_byte(seg, address) &&
             (memory::get_byte(seg, address) !=
              memory::get_byte(seg, startaddr)))))
            break;
        address++;
    }

    usize n = address - startaddr;

    if(n > 1 && n > static_cast<usize>(ctx->processorplugin->integer_size)) {
        memory::set_n(seg, startaddr, n, BF_DATA);
        memory::set_flag(seg, startaddr, BF_FILL);
    }
}

void process_function_graph(const Context* ctx,
                            std::vector<Function>& functions,
                            RDAddress address) {
    spdlog::info("Creating function graph @ {:x}", address);

    Function& f = functions.emplace_back(address);
    std::unordered_set<RDAddress> done;
    std::deque<RDAddress> pending;
    pending.push_back(address);

    while(!pending.empty()) {
        RDAddress startaddr = pending.front();
        pending.pop_front();

        // Ignore loops
        if(done.contains(startaddr)) continue;
        done.insert(startaddr);

        RDGraphNode n = f.try_add_block(startaddr);
        if(startaddr == address) f.graph.set_root(n);

        const RDSegment* seg = ctx->program.find_segment(startaddr);
        assume(seg);

        RDAddress endaddr = startaddr;

        // Find basic block end
        for(RDAddress curraddr = startaddr; curraddr < seg->end;) {
            if(!memory::has_flag(seg, curraddr, BF_CODE)) break;

            if(curraddr != startaddr) {
                if(memory::has_flag(seg, curraddr, BF_FUNCTION)) break;

                if(memory::has_flag(seg, curraddr, BF_JUMPDST)) {
                    pending.push_back(curraddr);
                    f.jmp_true(n, f.try_add_block(curraddr));
                    break;
                }
            }

            if(memory::has_flag(seg, curraddr, BF_JUMP)) {
                for(const RDRef& r :
                    ctx->get_refs_from_type(curraddr, CR_JUMP)) {
                    const RDSegment* jseg =
                        ctx->program.find_segment(r.address);

                    if(jseg && jseg->perm & SP_X) {
                        if(!memory::has_flag(jseg, r.address, BF_CODE))
                            continue;

                        pending.push_back(r.address);
                        f.jmp_true(n, f.try_add_block(r.address));
                    }
                }

                // Consume delay slot(s), if any
                while(curraddr < seg->end &&
                      memory::has_flag(seg, curraddr, BF_DFLOW)) {
                    usize len = memory::get_length(seg, curraddr);
                    if(!len) except("Invalid length @ {:x} (DS)", curraddr);
                    curraddr += len;
                }

                endaddr = curraddr;

                // Conditional Jump
                if(curraddr < seg->end &&
                   memory::has_flag(seg, curraddr, BF_FLOW)) {
                    usize len = memory::get_length(seg, curraddr);
                    if(!len) except("Invalid length @ {:x} (CJ)", curraddr);
                    curraddr += len;

                    if(curraddr < seg->end) {
                        pending.push_back(curraddr);
                        f.jmp_false(n, f.try_add_block(curraddr));
                    }
                }

                break;
            }

            endaddr = curraddr;
            usize len = memory::get_length(seg, curraddr);
            if(!len) except("Invalid length @ {:x} (END)", curraddr);
            if(!memory::has_flag(seg, curraddr, BF_FLOW)) break;
            curraddr += len;
        }

        Function::BasicBlock* bb = f.get_basic_block(n);
        assume(bb);
        bb->end = std::min<RDAddress>(endaddr, seg->end - 1);
    }
}

void process_listing_code(const Context* ctx, Listing& l,
                          std::vector<Function>& functions,
                          RDAddress& address) {
    const RDSegment* seg = l.current_segment();
    assume(seg);
    assume(memory::has_flag(seg, address, BF_CODE));

    if(memory::has_flag(seg, address, BF_FUNCTION)) {
        l.pop_indent(2);
        l.function(address);
        l.push_indent(2);

        const RDSegment* s = l.current_segment();
        assume(s);
        assume(s->perm & SP_X);
        memprocess::process_function_graph(ctx, functions, address);
    }
    else if(memory::has_flag(seg, address, BF_REFSTO)) {
        l.pop_indent();
        l.label(address);
        l.push_indent();
    }

    l.instruction(address);

    usize len = memory::get_length(seg, address);
    if(!len) except("Invalid length @ {:x} (CODE)", address);
    address += len;
}

void process_refsto(Context* ctx, const RDSegment* seg, RDAddress& address) {
    auto is_range_unkn = [&](RDAddress raddr, usize n) {
        return memory::range_is(seg, raddr, n,
                                [](RDMByte b) { return mbyte::is_unknown(b); });
    };

    Database::RefList refs = ctx->get_refs_to(address);
    std::unordered_set<usize> reftypes;
    for(const RDRef& r : refs)
        reftypes.insert(r.type);

    if(reftypes.contains(CR_JUMP) || reftypes.contains(CR_CALL)) {
        address++;
        return;
    }

    const RDProcessorPlugin* plugin = ctx->processorplugin;
    assume(plugin);
    auto addrtype = ctx->types.int_from_bytes(plugin->address_size);
    assume(addrtype.has_value());
    auto inttype = ctx->types.int_from_bytes(plugin->integer_size);
    assume(inttype.has_value());

    stringfinder::classify(address)
        .map([&](const RDStringResult& x) {
            if(x.terminated) {
                ctx->set_type(address, x.type, 0);
                address += x.totalsize;
            }
            else
                address++;
        })
        .or_else([&]() {
            if(reftypes.contains(DR_ADDRESS) &&
               is_range_unkn(address, plugin->address_size)) {
                ctx->set_type(address, *addrtype, 0);
                address += plugin->address_size;
            }
            else if(is_range_unkn(address, plugin->integer_size)) {
                ctx->set_type(address, *inttype, 0);
                address += plugin->integer_size;
            }
            else
                address++;
        });
}

void process_prologues() {
    Context* ctx = state::context;
    if(!ctx->processorplugin->get_prologues) return;

    const char** prologues =
        ctx->processorplugin->get_prologues(ctx->processor);
    if(!prologues) return;

    std::vector<pattern::Compiled> patterns = pattern::compile_all(prologues);
    if(patterns.empty()) return;

    spdlog::info("Finding function prologues...");
    ctx->worker->emulator.reset();

    for(const RDSegment& seg : ctx->program.segments) {
        if(!(seg.perm & SP_X)) continue;

        for(usize idx = 0; idx < seg.mem->length;) {
            usize l = 1;

            for(const pattern::Compiled& pat : patterns) {
                if(pattern::match_mbytes(pat, seg.mem, idx)) {
                    ctx->set_function(seg.start + idx, 0);
                    l = pat.size();
                    break;
                }
            }

            idx += l;
        }
    }
}

} // namespace

void merge_code(Emulator* e) {
    memprocess::process_prologues();

    const Context* ctx = state::context;

    for(const RDSegment& seg : ctx->program.segments) {
        //     if(!(seg.perm & SP_X) || seg.offset == seg.endoffset) continue;
        //
        //     usize idx = seg.index;
        //
        //     while(idx < seg.endindex && idx < mem->size()) {
        //         Byte b = mem->at(idx);
        //
        //         if(b.has_byte() && b.is_unknown()) {
        //             e->enqueue_flow(idx++);
        //
        //             // Move after the unknown range
        //             while(idx < seg.endindex && idx < mem->size() &&
        //                   mem->at(idx).is_unknown())
        //                 idx++;
        //         }
        //         else
        //             idx++;
        //     }
    }
}

void process_memory() {
    memprocess::process_regions();

    Context* ctx = state::context;
    std::vector<Function> f;

    for(RDSegment& seg : ctx->program.segments) {
        for(RDAddress address = seg.start; address < seg.end;) {

            if(memory::has_flag(&seg, address, BF_FUNCTION))
                memprocess::process_function_graph(ctx, f, address++);
            else if(memory::has_flag(&seg, address, BF_REFSTO))
                memprocess::process_refsto(ctx, &seg, address);
            else if(memory::is_unknown(&seg, address) &&
                    !memory::has_flag(&seg, address, BF_REFSFROM | BF_REFSTO)) {
                memprocess::process_unknown_data(ctx, &seg, address);
            }
            else
                address++;
        }
    }

    state::context->program.functions = std::move(f);
}

void process_listing() {
    memprocess::process_regions();

    const Context* ctx = state::context;
    assume(ctx);

    Listing l;
    std::vector<Function> f;

    for(const RDSegment& seg : ctx->program.segments) {
        l.segment(&seg);

        for(RDAddress address = seg.start; address < seg.end;) {
            if(memory::is_unknown(&seg, address)) {
                memprocess::process_listing_unknown(l, address);
                continue;
            }

            if(memory::has_flag(&seg, address, BF_DATA)) {
                memprocess::process_listing_data(ctx, l, address);
                continue;
            }

            if(memory::has_flag(&seg, address, BF_CODE)) {
                l.push_indent(4);
                memprocess::process_listing_code(ctx, l, f, address);
                l.pop_indent(4);
                continue;
            }

            unreachable;
        }
    }

    spdlog::info("Listing completed ({} items)", l.size());
    state::context->program.functions = std::move(f);
    state::context->listing = std::move(l);
}

} // namespace redasm::memprocess
