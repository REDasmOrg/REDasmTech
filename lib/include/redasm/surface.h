#pragma once

#include <redasm/common.h>
#include <redasm/theme.h>
#include <redasm/types.h>

RD_HANDLE(RDSurface);

typedef struct RDSurfaceLocation {
    struct {
        RDAddress value;
        bool valid;
    } address;

    struct {
        RDOffset value;
        bool valid;
    } offset;

    usize index;
    const char* segment;
} RDSurfaceLocation;

typedef struct RDSurfacePosition {
    usize row;
    usize col;
} RDSurfacePosition;

typedef struct RDSurfaceCell {
    char ch;
    RDThemeKind fg;
    RDThemeKind bg;
} RDSurfaceCell;

REDASM_EXPORT RDSurface* rdsurface_new();
REDASM_EXPORT void rdsurface_render(RDSurface* self, usize start, usize n);
REDASM_EXPORT bool rdsurface_hasselection(const RDSurface* self);
REDASM_EXPORT usize rdsurface_getrowcount(const RDSurface* self);
REDASM_EXPORT usize rdsurface_getrow(const RDSurface* self, usize idx,
                                     const RDSurfaceCell** row);
REDASM_EXPORT void rdsurface_getposition(const RDSurface* self,
                                         RDSurfacePosition* pos);
REDASM_EXPORT void rdsurface_getlocation(const RDSurface* self,
                                         RDSurfaceLocation* loc);
REDASM_EXPORT usize rdsurface_getindex(const RDSurface* self);
REDASM_EXPORT const char* rdsurface_gettext(const RDSurface* self);
REDASM_EXPORT const char* rdsurface_getselectedtext(const RDSurface* self);
REDASM_EXPORT void rdsurface_setcolumns(RDSurface* self, usize columns);
REDASM_EXPORT void rdsurface_setposition(RDSurface* self, usize row, usize col);
REDASM_EXPORT bool rdsurface_selectword(RDSurface* self, usize row, usize col);
REDASM_EXPORT bool rdsurface_select(RDSurface* self, usize row, usize col);
REDASM_EXPORT void rdsurface_seek(RDSurface* self, usize index);