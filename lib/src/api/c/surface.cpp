#include "../internal/surface.h"
#include <redasm/surface.h>

RDSurface* rdsurface_new(usize flags) {
    return redasm::api::internal::surface_new(flags);
}

bool rdsurface_getaddressundercursor(const RDSurface* self,
                                     RDAddress* address) {
    return redasm::api::internal::surface_getaddressundercursor(self, address);
}

void rdsurface_render(RDSurface* self, usize start, usize n) {
    redasm::api::internal::surface_render(self, start, n);
}

bool rdsurface_hasselection(const RDSurface* self) {
    return redasm::api::internal::surface_hasselection(self);
}

usize rdsurface_getrowcount(const RDSurface* self) {
    return redasm::api::internal::surface_getrowcount(self);
}

usize rdsurface_getrow(const RDSurface* self, usize idx,
                       const RDSurfaceCell** row) {
    return redasm::api::internal::surface_getrow(self, idx, row);
}

void rdsurface_getposition(const RDSurface* self, RDSurfacePosition* pos) {
    redasm::api::internal::surface_getposition(self, pos);
}

void rdsurface_getlocation(const RDSurface* self, RDSurfaceLocation* loc) {
    redasm::api::internal::surface_getlocation(self, loc);
}

usize rdsurface_getindex(const RDSurface* self) {
    return redasm::api::internal::surface_getindex(self);
}

const char* rdsurface_gettext(const RDSurface* self) {
    return redasm::api::internal::surface_gettext(self).data();
}

const char* rdsurface_getselectedtext(const RDSurface* self) {
    return redasm::api::internal::surface_getselectedtext(self).data();
}

void rdsurface_setcolumns(RDSurface* self, usize columns) {
    redasm::api::internal::surface_setcolumns(self, columns);
}

void rdsurface_setposition(RDSurface* self, usize row, usize col) {
    redasm::api::internal::surface_setposition(self, row, col);
}

bool rdsurface_selectword(RDSurface* self, usize row, usize col) {
    return redasm::api::internal::surface_selectword(self, row, col);
}

bool rdsurface_select(RDSurface* self, usize row, usize col) {
    return redasm::api::internal::surface_select(self, row, col);
}

void rdsurface_seek(RDSurface* self, usize index) {
    redasm::api::internal::surface_seek(self, index);
}
