#include "surface.h"
#include "../../context.h"
#include "../../state.h"
#include "../../surface/surface.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

RDSurface* surface_new(usize flags) { return api::to_c(new Surface(flags)); }

void surface_render(RDSurface* self, usize start, usize n) {
    spdlog::trace("surface_render({}, {}, {})", fmt::ptr(self), start, n);
    api::from_c(self)->render(start, n);
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
                     const RDSurfaceCell** row) {
    spdlog::trace("surface_getrow({}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(row));

    const Surface* s = api::from_c(self);

    if(idx >= s->rows.size())
        return 0;

    if(row)
        *row = s->rows[idx].cells.data();

    return s->rows[idx].cells.size();
}

void surface_getposition(const RDSurface* self, RDSurfacePosition* pos) {
    spdlog::trace("surface_getposition({}, {})", fmt::ptr(self), fmt::ptr(pos));

    if(pos)
        *pos = api::from_c(self)->position();
}

bool surface_getindex(const RDSurface* self, usize* index) {
    spdlog::trace("surface_getindex({})", fmt::ptr(self));
    auto idx = api::from_c(self)->current_index();
    if(*index)
        *index = *idx;
    return idx.has_value();
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

        const Function* func = s->current_function();

        if(func) {
            auto ep = ctx->index_to_address(func->entry);

            if(ep)
                loc->function.value = *ep;

            loc->function.valid = ep.has_value();
        }
        else
            loc->function.valid = false;

        auto address = ctx->index_to_address(*index);
        auto offset = ctx->index_to_offset(*index);

        if(address)
            loc->address.value = *address;

        if(offset)
            loc->offset.value = *offset;

        loc->address.valid = address.has_value();
        loc->offset.valid = offset.has_value();
    }
    else {
        loc->function.valid = false;
        loc->index.valid = false;
        loc->address.valid = false;
        loc->offset.valid = false;
    }

    loc->listingindex.value = s->start;
    loc->listingindex.valid = s->start < ctx->listing.size();

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

bool surface_getaddressundercursor(const RDSurface* self, RDAddress* address) {
    spdlog::trace("surface_getaddressundercursor({}, {})", fmt::ptr(self),
                  fmt::ptr(address));

    auto addr = api::from_c(self)->index_under_cursor().and_then(
        [](usize idx) { return state::context->index_to_address(idx); });

    if(address && addr)
        *address = *addr;

    return addr.has_value();
}

void surface_setcolumns(RDSurface* self, usize columns) {
    spdlog::trace("surface_setcolumns({}, {})", fmt::ptr(self), columns);
    api::from_c(self)->set_columns(columns);
}

void surface_setposition(RDSurface* self, usize row, usize col) {
    spdlog::trace("surface_setposition({}, {}, {})", fmt::ptr(self), row, col);
    api::from_c(self)->set_position(row, col);
}

bool surface_selectword(RDSurface* self, usize row, usize col) {
    spdlog::trace("surface_selectword({}, {}, {})", fmt::ptr(self), row, col);
    return api::from_c(self)->select_word(row, col);
}

bool surface_select(RDSurface* self, usize row, usize col) {
    spdlog::trace("surface_select({}, {}, {})", fmt::ptr(self), row, col);
    return api::from_c(self)->select(row, col);
}

void surface_seektoep(RDSurface* self) {
    spdlog::trace("surface_seektoep({}, {}, {})", fmt::ptr(self));
    return api::from_c(self)->seek_to_ep();
}

void surface_seek(RDSurface* self, usize index) {
    spdlog::trace("surface_seek({}, {})", fmt::ptr(self), index);
    api::from_c(self)->seek(index);
}

} // namespace redasm::api::internal
