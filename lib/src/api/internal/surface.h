#pragma once

#include <redasm/surface.h>
#include <redasm/types.h>
#include <string_view>

namespace redasm::api::internal {

RDSurface* surface_new();
void surface_render(RDSurface* self, usize start, usize n);
bool surface_hasselection(const RDSurface* self);
usize surface_getrowcount(const RDSurface* self);
usize surface_getrow(const RDSurface* self, usize idx,
                     const RDSurfaceCell** row);
void surface_getposition(const RDSurface* self, RDSurfacePosition* pos);
void surface_getlocation(const RDSurface* self, RDSurfaceLocation* loc);
usize surface_getindex(const RDSurface* self);
std::string_view surface_gettext(const RDSurface* self);
std::string_view surface_getselectedtext(const RDSurface* self);
void surface_setcolumns(RDSurface* self, usize columns);
void surface_setposition(RDSurface* self, usize row, usize col);
bool surface_selectword(RDSurface* self, usize row, usize col);
bool surface_select(RDSurface* self, usize row, usize col);
void surface_seek(RDSurface* self, usize index);

} // namespace redasm::api::internal