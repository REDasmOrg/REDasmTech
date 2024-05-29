#include "listing.h"
#include "../../context.h"
#include "../../listing.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

bool listing_getsymbol(usize idx, RDSymbol* symbol) {
    spdlog::trace("listing_getsymbol({}, {})", idx, fmt::ptr(symbol));

    static std::string value;

    const Context* ctx = state::context;
    const Listing& listing = ctx->listing;
    const Listing::IndexList& symbols = listing.symbols();

    if(idx >= symbols.size())
        return false;

    usize listingidx = symbols[idx];
    assume(listingidx < listing.size());

    if(symbol) {
        const Database& db = ctx->database;
        const ListingItem& item = listing[listingidx];

        symbol->address = ctx->baseaddress + item.index;
        symbol->index = item.index;

        switch(item.type) {
            case ListingItemType::SEGMENT: {
                const AddressDetail& d = db.get_detail(item.index);
                const Segment& s = ctx->segments.at(d.segment_index);
                symbol->type = SYMBOL_SEGMENT;
                symbol->theme = THEME_SEGMENT;
                symbol->value = s.name.c_str();
                break;
            }

            case ListingItemType::FUNCTION: {
                value = ctx->get_name(item.index);
                symbol->type = SYMBOL_FUNCTION;
                symbol->theme = THEME_FUNCTION;
                symbol->value = value.c_str();
                break;
            }

            case ListingItemType::TYPE: {
                tl::optional<std::string> s;
                assume(item.parsed_type);

                switch(item.parsed_type->type->id()) {
                    case typing::types::STR:
                        s = state::context->memory->get_string(item.index);
                        break;

                    case typing::types::WSTR:
                        s = state::context->memory->get_wstring(item.index);
                        break;

                    case typing::types::CHAR:
                        spdlog::critical("UNHANDLED CHAR");
                        break;

                    case typing::types::WCHAR:
                        spdlog::critical("UNHANDLED WCHAR");
                        break;

                    default: break;
                }

                if(!s) {
                    value = ctx->get_name(item.index);

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

            case ListingItemType::ARRAY: {
                value = ctx->get_name(item.index);

                symbol->type = SYMBOL_ARRAY;
                symbol->theme = THEME_DEFAULT;
                symbol->value = value.c_str();
                break;
            }

            default: unreachable; break;
        }
    }

    return true;
}

tl::optional<usize> listing_getindex(RDAddress address) {
    spdlog::trace("listing_getindex({:x})", address);

    auto idx = state::context->address_to_index(address);
    if(!idx)
        return tl::nullopt;

    auto it = state::context->listing.lower_bound(*idx);
    if(it == state::context->listing.end())
        return tl::nullopt;

    return std::distance(state::context->listing.cbegin(), it);
}

usize listing_getsymbolslength() {
    spdlog::trace("listing_getsymbolslength()");
    return state::context->listing.symbols().size();
}

usize listing_getlength() {
    spdlog::trace("listing_getlength()");
    return state::context->listing.size();
}

} // namespace redasm::api::internal
