#include "../listing.h"
#include "../context.h"
#include "../memory/memory.h"
#include "../state.h"
#include <redasm/listing.h>

namespace {

void listingindex_tosymbol(LIndex lidx, RDSymbol* symbol, std::string& value) {
    ct_assume(symbol);
    const redasm::Context* ctx = redasm::state::context;
    ct_assume(ctx);
    const redasm::Listing& listing = ctx->listing;
    const redasm::ListingItem& item = listing[lidx];

    symbol->address = item.address;

    switch(item.type) {
        case LISTINGITEM_SEGMENT: {
            const RDSegment* seg = ctx->program.find_segment(item.address);
            ct_assume(seg);
            symbol->type = SYMBOL_SEGMENT;
            symbol->theme = THEME_SEGMENT;
            symbol->value = seg->name;
            break;
        }

        case LISTINGITEM_FUNCTION: {
            value = ctx->get_name(item.address);
            symbol->type = SYMBOL_FUNCTION;
            symbol->theme = THEME_FUNCTION;
            symbol->value = value.c_str();
            break;
        }

        case LISTINGITEM_TYPE: {
            const RDSegment* seg = ctx->program.find_segment(item.address);
            ct_assume(seg);
            tl::optional<std::string> s;
            ct_assume(item.dtype);

            if(item.dtype->def->kind == TK_PRIMITIVE) {
                switch(item.dtype->def->t_primitive) {
                    case T_STR:
                        s = redasm::memory::get_str(seg, item.address);
                        break;

                    case T_WSTR:
                        s = redasm::memory::get_wstr(seg, item.address);
                        break;

                    case T_CHAR: {
                        usize len =
                            redasm::memory::get_length(seg, item.address);
                        ct_assume(len > 0);
                        s = redasm::memory::get_str(seg, item.address, len);
                        break;
                    }

                    case T_WCHAR: {
                        usize len =
                            redasm::memory::get_length(seg, item.address);
                        ct_assume(len > 0);
                        s = redasm::memory::get_wstr(seg, item.address, len);
                        break;
                    }

                    default: break;
                }
            }

            if(!s) {
                value = ctx->get_name(item.address);

                symbol->type = SYMBOL_TYPE;
                symbol->theme = THEME_DEFAULT;
                symbol->value = value.c_str();
            }
            else {
                value = "\"" + *s + "\"";

                symbol->type = SYMBOL_STRING;
                symbol->theme = THEME_STRING;
                symbol->value = value.c_str();
            }

            break;
        }

        default: ct_unreachable; break;
    }
}

} // namespace

bool rdlisting_getindex(RDAddress address, LIndex* idx) {
    spdlog::trace("rdlisting_index({:x}, {})", address, fmt::ptr(idx));

    if(!redasm::state::context) return false;

    auto it = redasm::state::context->listing.lower_bound(address);
    if(it == redasm::state::context->listing.end()) return false;

    if(idx) *idx = std::distance(redasm::state::context->listing.cbegin(), it);
    return true;
}

bool rdlisting_getsymbol(usize idx, RDSymbol* symbol) {
    spdlog::trace("rdlisting_getsymbol({}, {})", idx, fmt::ptr(symbol));

    static std::string value;
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    const redasm::Listing& listing = ctx->listing;
    const redasm::Listing::LIndexList& symbols = listing.symbols();
    if(idx >= symbols.size()) return false;

    LIndex lidx = symbols[idx];
    ct_assume(lidx < listing.size());
    if(symbol) listingindex_tosymbol(lidx, symbol, value);
    return true;
}

usize rdlisting_getsymbolslength() {
    spdlog::trace("rdlisting_getsymbolslength()");
    return redasm::state::context
               ? redasm::state::context->listing.symbols().size()
               : 0;
}

bool rdlisting_getimport(usize idx, RDSymbol* symbol) {
    spdlog::trace("rdlisting_getsymbolslength({}, {})", idx, fmt::ptr(symbol));

    static std::string value;
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    const redasm::Listing& listing = ctx->listing;
    const redasm::Listing::LIndexList& imports = listing.imports();
    if(idx >= imports.size()) return false;

    usize listingidx = imports[idx];
    ct_assume(listingidx < listing.size());
    if(symbol) listingindex_tosymbol(listingidx, symbol, value);
    return true;
}

usize rdlisting_getimportslength() {
    spdlog::trace("rdlisting_getimportslength()");
    return redasm::state::context
               ? redasm::state::context->listing.imports().size()
               : 0;
}

bool rdlisting_getexport(usize idx, RDSymbol* symbol) {
    spdlog::trace("rdlisting_getexport({}, {})", idx, fmt::ptr(symbol));

    static std::string value;
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    const redasm::Listing& listing = ctx->listing;
    const redasm::Listing::LIndexList& exports = listing.exports();
    if(idx >= exports.size()) return false;

    usize listingidx = exports[idx];
    ct_assume(listingidx < listing.size());
    if(symbol) listingindex_tosymbol(listingidx, symbol, value);
    return true;
}

usize rdlisting_getexportslength() {
    spdlog::trace("rdlisting_getexportslength()");
    return redasm::state::context
               ? redasm::state::context->listing.exports().size()
               : 0;
}

usize rdlisting_getlength() {
    spdlog::trace("rdlisting_getlength()");
    return redasm::state::context ? redasm::state::context->listing.size() : 0;
}
