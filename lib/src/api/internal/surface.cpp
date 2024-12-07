#include "surface.h"
#include "../../context.h"
#include "../../state.h"
#include "../../surface/surface.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

RDSurface* surface_new(usize flags) { return api::to_c(new Surface(flags)); }

void surface_render(RDSurface* self, usize n) {
    spdlog::trace("surface_render({}, {})", fmt::ptr(self), n);
    api::from_c(self)->render(n);
}

void surface_renderfunction(RDSurface* self, const RDFunction* f) {
    spdlog::trace("surface_renderfunction({}, {})", fmt::ptr(self),
                  fmt::ptr(f));
    if(f)
        api::from_c(self)->render_function(*api::from_c(f));
}

bool surface_hasselection(const RDSurface* self) {
    spdlog::trace("surface_hasselection({})", fmt::ptr(self));
    return api::from_c(self)->has_selection();
}

usize surface_getrowcount(const RDSurface* self) {
    spdlog::trace("surface_getrowcount({})", fmt::ptr(self));
    return api::from_c(self)->rows.size();
}

usize surface_getrow(const RDSurface* self, usize idx,
                     const RDSurfaceCell** row, usize* len) {
    spdlog::trace("surface_getrow({}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(row));

    const Surface* s = api::from_c(self);

    if(idx >= s->rows.size())
        return 0;

    if(row)
        *row = s->rows[idx].cells.data();

    if(len)
        *len = s->rows[idx].length;

    return s->rows[idx].cells.size();
}

void surface_getposition(const RDSurface* self, RDSurfacePosition* pos) {
    spdlog::trace("surface_getposition({}, {})", fmt::ptr(self), fmt::ptr(pos));

    if(pos)
        *pos = api::from_c(self)->position();
}

bool surface_getindex(const RDSurface* self, MIndex* index) {
    spdlog::trace("surface_getindex({})", fmt::ptr(self));
    auto idx = api::from_c(self)->current_index();
    if(*index)
        *index = *idx;
    return idx.has_value();
}

int surface_indexof(const RDSurface* self, RDAddress address) {
    spdlog::trace("surface_indexof({}, {:x})", fmt::ptr(self), address);

    if(auto idx = state::context->address_to_index(address); idx)
        return api::from_c(self)->index_of(*idx);

    return -1;
}

int surface_lastindexof(const RDSurface* self, RDAddress address) {
    spdlog::trace("surface_lastindexof({}, {:x})", fmt::ptr(self), address);

    if(auto idx = state::context->address_to_index(address); idx)
        return api::from_c(self)->last_index_of(*idx);

    return -1;
}

usize surface_getpath(const RDSurface* self, const RDSurfacePath** path) {
    spdlog::trace("surface_getpath({}, {})", fmt::ptr(self), fmt::ptr(path));
    const Surface* s = api::from_c(self);

    if(path)
        *path = s->get_path().data();

    return s->get_path().size();
}

void surface_getlocation(const RDSurface* self, RDSurfaceLocation* loc) {
    spdlog::trace("surface_getlocation({}, {})", fmt::ptr(self), fmt::ptr(loc));

    if(!loc)
        return;

    Context* ctx = state::context;
    const Surface* s = api::from_c(self);
    const Segment* seg = s->current_segment();
    auto index = s->current_index();

    if(index) {
        loc->index.value = *index;
        loc->index.valid = true;

        auto cursorindex = s->index_under_cursor();

        if(cursorindex) {
            auto cursoraddress = ctx->index_to_address(*cursorindex);
            cursoraddress.map(
                [&](RDAddress address) { loc->cursoraddress.value = address; });
            loc->cursoraddress.valid = cursoraddress.has_value();
        }
        else
            loc->cursoraddress.valid = false;

        const Function* func = s->current_function();

        if(func) {
            auto ep = ctx->index_to_address(func->index);
            ep.map([&](RDAddress ep) { loc->function.value = ep; });
            loc->function.valid = ep.has_value();
        }
        else
            loc->function.valid = false;

        auto address = ctx->index_to_address(*index);
        address.map([&](RDAddress address) { loc->address.value = address; });
        loc->address.valid = address.has_value();

        auto offset = ctx->index_to_offset(*index);
        offset.map([&](RDOffset offset) { loc->offset.value = offset; });
        loc->offset.valid = offset.has_value();
    }
    else {
        loc->function.valid = false;
        loc->index.valid = false;
        loc->cursoraddress.valid = false;
        loc->address.valid = false;
        loc->offset.valid = false;
    }

    if(s->start) {
        loc->startindex.value = *s->start;
        loc->startindex.valid = s->start < ctx->listing.size();

        s->current_listing_index()
            .map([&](LIndex index) {
                loc->listingindex.valid = index < ctx->listing.size();

                if(loc->listingindex.valid) {
                    const ListingItem& item = ctx->listing[index];
                    loc->listingindex.type = item.type;
                    loc->listingindex.value = index;
                }
            })
            .or_else([&]() { loc->listingindex.valid = false; });
    }
    else {
        loc->startindex.valid = false;
        loc->listingindex.valid = false;
    }

    loc->segment = seg ? seg->name.c_str() : nullptr;
}

std::string_view surface_gettext(const RDSurface* self) {
    spdlog::trace("surface_gettext({})", fmt::ptr(self));
    return api::from_c(self)->get_text();
}

std::string_view surface_getselectedtext(const RDSurface* self) {
    spdlog::trace("surface_getselectedtext({})", fmt::ptr(self));
    return api::from_c(self)->get_selected_text();
}

bool surface_getaddressunderpos(const RDSurface* self,
                                const RDSurfacePosition* pos,
                                RDAddress* address) {
    spdlog::trace("surface_getaddressunderpos({}, {}, {})", fmt::ptr(pos),
                  fmt::ptr(self), fmt::ptr(address));

    if(!pos)
        return false;

    auto addr = api::from_c(self)->index_under_pos(*pos).and_then(
        [](usize idx) { return state::context->index_to_address(idx); });

    if(address && addr)
        *address = *addr;

    return addr.has_value();
}

bool surface_getaddressundercursor(const RDSurface* self, RDAddress* address) {
    spdlog::trace("surface_getaddressundercursor({}, {})", fmt::ptr(self),
                  fmt::ptr(address));

    auto addr = api::from_c(self)->index_under_cursor().and_then(
        [](usize idx) { return state::context->index_to_address(idx); });

    if(address && addr)
        *address = *addr;

    return addr.has_value();
}

bool surface_hasrdil(const RDSurface* self) {
    spdlog::trace("surface_hasrdil({})", fmt::ptr(self));
    return api::from_c(self)->has_rdil();
}

void surface_setrdil(RDSurface* self, bool v) {
    spdlog::trace("surface_setrdil({}, {})", fmt::ptr(self), v);
    api::from_c(self)->set_rdil(v);
}

void surface_setcolumns(RDSurface* self, usize columns) {
    spdlog::trace("surface_setcolumns({}, {})", fmt::ptr(self), columns);
    api::from_c(self)->set_columns(columns);
}

void surface_setposition(RDSurface* self, int row, int col) {
    spdlog::trace("surface_setposition({}, {}, {})", fmt::ptr(self), row, col);
    api::from_c(self)->set_position(row, col);
}

bool surface_selectword(RDSurface* self, int row, int col) {
    spdlog::trace("surface_selectword({}, {}, {})", fmt::ptr(self), row, col);
    return api::from_c(self)->select_word(row, col);
}

bool surface_select(RDSurface* self, int row, int col) {
    spdlog::trace("surface_select({}, {}, {})", fmt::ptr(self), row, col);
    return api::from_c(self)->select(row, col);
}

void surface_clearselection(RDSurface* self) {
    spdlog::trace("surface_clearselection({})", fmt::ptr(self));
    api::from_c(self)->clear_selection();
}

void surface_clearhistory(RDSurface* self) {
    spdlog::trace("surface_clearhistory({})", fmt::ptr(self));
    api::from_c(self)->clear_history();
}

bool surface_jumpto(RDSurface* self, MIndex index) {
    spdlog::trace("surface_jumpto({}, {})", fmt::ptr(self), index);
    return api::from_c(self)->jump_to(index);
}

bool surface_jumptoep(RDSurface* self) {
    spdlog::trace("surface_jumptoep({})", fmt::ptr(self));
    return api::from_c(self)->jump_to_ep();
}

void surface_seekposition(RDSurface* self, LIndex index) {
    spdlog::trace("surface_seekposition({}, {})", fmt::ptr(self), index);
    api::from_c(self)->seek_position(index);
}

void surface_seek(RDSurface* self, LIndex index) {
    spdlog::trace("surface_seek({}, {})", fmt::ptr(self), index);
    api::from_c(self)->seek(index);
}

bool surface_cangoback(const RDSurface* self) {
    spdlog::trace("surface_cangoback({})", fmt::ptr(self));
    return api::from_c(self)->can_goback();
}

bool surface_cangoforward(const RDSurface* self) {
    spdlog::trace("surface_cangoforward({})", fmt::ptr(self));
    return api::from_c(self)->can_goforward();
}

bool surface_goback(RDSurface* self) {
    spdlog::trace("surface_goback({})", fmt::ptr(self));
    return api::from_c(self)->go_back();
}

bool surface_goforward(RDSurface* self) {
    spdlog::trace("surface_goforward({})", fmt::ptr(self));
    return api::from_c(self)->go_forward();
}

} // namespace redasm::api::internal
