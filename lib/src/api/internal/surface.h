#pragma once

#include <redasm/surface.h>
#include <redasm/types.h>
#include <string_view>

namespace redasm::api::internal {

RDSurface* surface_new(usize flags);
void surface_render(RDSurface* self, usize n);
bool surface_hasselection(const RDSurface* self);
usize surface_getrowcount(const RDSurface* self);
usize surface_getrow(const RDSurface* self, usize idx,
                     const RDSurfaceCell** row);
void surface_getposition(const RDSurface* self, RDSurfacePosition* pos);
void surface_getlocation(const RDSurface* self, RDSurfaceLocation* loc);
bool surface_getindex(const RDSurface* self, usize* index);
usize surface_getpath(const RDSurface* self, const RDSurfacePath** path);
std::string_view surface_gettext(const RDSurface* self);
std::string_view surface_getselectedtext(const RDSurface* self);
bool surface_getaddressundercursor(const RDSurface* self, RDAddress* address);
void surface_setcolumns(RDSurface* self, usize columns);
void surface_setposition(RDSurface* self, usize row, usize col);
bool surface_selectword(RDSurface* self, usize row, usize col);
bool surface_select(RDSurface* self, usize row, usize col);
void surface_clearselection(RDSurface* self);
void surface_seek(RDSurface* self, LIndex index);
bool surface_jumpto(RDSurface* self, MIndex index);
bool surface_jumptoep(RDSurface* self);

} // namespace redasm::api::internal
