#include "../surface/surface.h"
#include "../context.h"
#include "../state.h"
#include "marshal.h"
#include <redasm/surface.h>
#include <spdlog/spdlog.h>

RDSurface* rdsurface_create(usize flags) {
    spdlog::trace("rdsurface_new({:x})", flags);
    return redasm::api::to_c(new redasm::Surface(flags));
}

void rdsurface_destroy(RDSurface* self) {
    spdlog::trace("rdsurface_destroy({})", fmt::ptr(self));
    delete redasm::api::from_c(self);
}

bool rdsurface_getaddressunderpos(const RDSurface* self,
                                  const RDSurfacePosition* pos,
                                  RDAddress* address) {
    spdlog::trace("rdsurface_getaddressunderpos({}, {}, {})", fmt::ptr(pos),
                  fmt::ptr(self), fmt::ptr(address));

    if(!pos) return false;

    auto addr = redasm::api::from_c(self)->address_under_pos(*pos);
    if(address && addr) *address = *addr;
    return addr.has_value();
}

bool rdsurface_getaddressundercursor(const RDSurface* self,
                                     RDAddress* address) {
    spdlog::trace("rdsurface_getaddressundercursor({}, {})", fmt::ptr(self),
                  fmt::ptr(address));

    auto addr = redasm::api::from_c(self)->address_under_cursor();
    if(address && addr) *address = *addr;
    return addr.has_value();
}

void rdsurface_render(RDSurface* self, usize n) {
    spdlog::trace("rdsurface_render({}, {})", fmt::ptr(self), n);
    redasm::api::from_c(self)->render(n);
}

void rdsurface_renderfunction(RDSurface* self, const RDFunction* f) {
    spdlog::trace("rdsurface_renderfunction({}, {})", fmt::ptr(self),
                  fmt::ptr(f));
    if(f) redasm::api::from_c(self)->render_function(*redasm::api::from_c(f));
}

bool rdsurface_hasselection(const RDSurface* self) {
    spdlog::trace("rdsurface_hasselection({})", fmt::ptr(self));
    return redasm::api::from_c(self)->has_selection();
}

usize rdsurface_getrowcount(const RDSurface* self) {
    spdlog::trace("rdsurface_getrowcount({})", fmt::ptr(self));
    return redasm::api::from_c(self)->rows.size();
}

usize rdsurface_getpath(const RDSurface* self, const RDSurfacePath** path) {
    spdlog::trace("rdsurface_getpath({}, {})", fmt::ptr(self), fmt::ptr(path));
    const redasm::Surface* s = redasm::api::from_c(self);
    if(path) *path = s->get_path().data();
    return s->get_path().size();
}

usize rdsurface_getrow(const RDSurface* self, usize idx,
                       const RDSurfaceCell** row, usize* len) {
    spdlog::trace("rdsurface_getrow({}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(row));
    const redasm::Surface* s = redasm::api::from_c(self);
    if(idx >= s->rows.size()) return 0;
    if(row) *row = s->rows[idx].cells.data();
    if(len) *len = s->rows[idx].length;
    return s->rows[idx].cells.size();
}

void rdsurface_getposition(const RDSurface* self, RDSurfacePosition* pos) {
    spdlog::trace("rdsurface_getposition({}, {})", fmt::ptr(self),
                  fmt::ptr(pos));
    if(pos) *pos = redasm::api::from_c(self)->position();
}

void rdsurface_getlocation(const RDSurface* self, RDSurfaceLocation* loc) {
    spdlog::trace("rdsurface_getlocation({}, {})", fmt::ptr(self),
                  fmt::ptr(loc));

    if(!loc) return;
    *loc = {}; // Initialize struct to 0

    const redasm::Context* ctx = redasm::state::context;
    const redasm::Surface* s = redasm::api::from_c(self);

    loc->segment = s->current_segment();
    auto address = s->current_address();

    if(loc->segment && address) {
        loc->address.value = *address;
        loc->address.valid = true;

        if(auto cursoraddress = s->address_under_cursor(); cursoraddress) {
            loc->cursoraddress.value = *cursoraddress;
            loc->cursoraddress.valid = true;
        }
        else
            loc->cursoraddress.valid = false;

        if(const redasm::Function* func = s->current_function(); func) {
            loc->function.value = func->address;
            loc->function.valid = true;
        }

        auto offset = ctx->program.to_offset(*address);
        offset.map([&](RDOffset offset) { loc->offset.value = offset; });
        loc->offset.valid = offset.has_value();
    }

    if(s->start) {
        loc->startindex.value = *s->start;
        loc->startindex.valid = s->start < ctx->listing.size();

        s->current_listing_index().map([&](LIndex index) {
            loc->listingindex.valid = index < ctx->listing.size();

            if(loc->listingindex.valid) {
                const redasm::ListingItem& item = ctx->listing[index];
                loc->listingindex.type = item.type;
                loc->listingindex.value = index;
            }
        });
    }
}

bool rdsurface_getaddress(const RDSurface* self, RDAddress* address) {
    spdlog::trace("rdsurface_getindex({})", fmt::ptr(self));
    auto addr = redasm::api::from_c(self)->current_address();
    if(address) *address = *addr;
    return addr.has_value();
}

int rdsurface_indexof(const RDSurface* self, RDAddress address) {
    spdlog::trace("rdsurface_indexof({}, {:x})", fmt::ptr(self), address);
    return redasm::api::from_c(self)->index_of(address);
}

int rdsurface_lastindexof(const RDSurface* self, RDAddress address) {
    spdlog::trace("rdsurface_lastindexof({}, {:x})", fmt::ptr(self), address);
    return redasm::api::from_c(self)->last_index_of(address);
}

const char* rdsurface_gettext(const RDSurface* self) {
    spdlog::trace("rdsurface_gettext({})", fmt::ptr(self));
    return redasm::api::from_c(self)->get_text().data();
}

const char* rdsurface_getselectedtext(const RDSurface* self) {
    spdlog::trace("rdsurface_getselectedtext({})", fmt::ptr(self));
    return redasm::api::from_c(self)->get_selected_text().data();
}

bool rdsurface_hasrdil(const RDSurface* self) {
    spdlog::trace("rdsurface_hasrdil({})", fmt::ptr(self));
    return redasm::api::from_c(self)->has_rdil();
}

void rdsurface_setrdil(RDSurface* self, bool v) {
    spdlog::trace("rdsurface_setrdil({}, {})", fmt::ptr(self), v);
    redasm::api::from_c(self)->set_rdil(v);
}

void rdsurface_setcolumns(RDSurface* self, usize columns) {
    spdlog::trace("rdsurface_setcolumns({}, {})", fmt::ptr(self), columns);
    redasm::api::from_c(self)->set_columns(columns);
}

void rdsurface_setposition(RDSurface* self, int row, int col) {
    spdlog::trace("rdsurface_setposition({}, {}, {})", fmt::ptr(self), row,
                  col);
    redasm::api::from_c(self)->set_position(row, col);
}

bool rdsurface_selectword(RDSurface* self, int row, int col) {
    spdlog::trace("rdsurface_selectword({}, {}, {})", fmt::ptr(self), row, col);
    return redasm::api::from_c(self)->select_word(row, col);
}

bool rdsurface_select(RDSurface* self, int row, int col) {
    spdlog::trace("rdsurface_select({}, {}, {})", fmt::ptr(self), row, col);
    return redasm::api::from_c(self)->select(row, col);
}

void rdsurface_clearselection(RDSurface* self) {
    spdlog::trace("rdsurface_clearselection({})", fmt::ptr(self));
    redasm::api::from_c(self)->clear_selection();
}

void rdsurface_clearhistory(RDSurface* self) {
    spdlog::trace("rdsurface_clearhistory({})", fmt::ptr(self));
    redasm::api::from_c(self)->clear_history();
}

void rdsurface_seekposition(RDSurface* self, LIndex index) {
    spdlog::trace("rdsurface_seekposition({}, {})", fmt::ptr(self), index);
    redasm::api::from_c(self)->seek_position(index);
}

void rdsurface_seek(RDSurface* self, LIndex index) {
    spdlog::trace("rdsurface_seek({}, {})", fmt::ptr(self), index);
    redasm::api::from_c(self)->seek(index);
}

bool rdsurface_jumptoep(RDSurface* self) {
    spdlog::trace("rdsurface_jumptoep({})", fmt::ptr(self));
    return redasm::api::from_c(self)->jump_to_ep();
}

bool rdsurface_cangoback(const RDSurface* self) {
    spdlog::trace("rdsurface_cangoback({})", fmt::ptr(self));
    return redasm::api::from_c(self)->can_goback();
}

bool rdsurface_cangoforward(const RDSurface* self) {
    spdlog::trace("rdsurface_cangoforward({})", fmt::ptr(self));
    return redasm::api::from_c(self)->can_goforward();
}

bool rdsurface_goback(RDSurface* self) {
    spdlog::trace("rdsurface_goback({})", fmt::ptr(self));
    return redasm::api::from_c(self)->go_back();
}

bool rdsurface_goforward(RDSurface* self) {
    spdlog::trace("rdsurface_goforward({})", fmt::ptr(self));
    return redasm::api::from_c(self)->go_forward();
}
