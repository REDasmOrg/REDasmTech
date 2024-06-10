#pragma once

#include <redasm/common.h>
#include <redasm/theme.h>
#include <redasm/types.h>

RD_HANDLE(RDSurface);

typedef enum RDSurfaceFlags {
    SURFACE_DEFAULT = 0u,
    SURFACE_NOADDRESS = 1u << 0,
    SURFACE_NOREFS = 1u << 1,
    SURFACE_NOCOMMENTS = 1u << 2,
    SURFACE_NOCURSOR = 1u << 3,
    SURFACE_NOCURSORLINE = 1u << 4,
    SURFACE_NOSELECTION = 1u << 5,
    SURFACE_NOHIGHLIGHT = 1u << 6,

    SURFACE_TEXT = ~0u,
} RDSurfaceFlags;

typedef struct RDSurfaceLocation {
    struct {
        usize value;
        bool valid;
    } function;

    struct {
        RDAddress value;
        bool valid;
    } address;

    struct {
        RDOffset value;
        bool valid;
    } offset;

    struct {
        usize value;
        bool valid;
    } index;

    struct {
        usize value;
        bool valid;
    } listingindex;

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

typedef struct RDSurfacePath {
    int fromrow; // = -1 if out of range
    int torow;   // = rows + 1 if out of range
    u8 style;
} RDSurfacePath;

REDASM_EXPORT RDSurface* rdsurface_new(usize flags);
REDASM_EXPORT void rdsurface_render(RDSurface* self, usize start, usize n);
REDASM_EXPORT bool rdsurface_hasselection(const RDSurface* self);
REDASM_EXPORT usize rdsurface_getrowcount(const RDSurface* self);
REDASM_EXPORT usize rdsurface_getpath(const RDSurface* self,
                                      const RDSurfacePath** path);
REDASM_EXPORT usize rdsurface_getrow(const RDSurface* self, usize idx,
                                     const RDSurfaceCell** row);
REDASM_EXPORT void rdsurface_getposition(const RDSurface* self,
                                         RDSurfacePosition* pos);
REDASM_EXPORT void rdsurface_getlocation(const RDSurface* self,
                                         RDSurfaceLocation* loc);
REDASM_EXPORT bool rdsurface_getindex(const RDSurface* self, usize* index);
REDASM_EXPORT const char* rdsurface_gettext(const RDSurface* self);
REDASM_EXPORT const char* rdsurface_getselectedtext(const RDSurface* self);
REDASM_EXPORT bool rdsurface_getaddressundercursor(const RDSurface* self,
                                                   RDAddress* address);
REDASM_EXPORT void rdsurface_setcolumns(RDSurface* self, usize columns);
REDASM_EXPORT void rdsurface_setposition(RDSurface* self, usize row, usize col);
REDASM_EXPORT bool rdsurface_selectword(RDSurface* self, usize row, usize col);
REDASM_EXPORT bool rdsurface_select(RDSurface* self, usize row, usize col);
REDASM_EXPORT void rdsurface_seek(RDSurface* self, usize index);
REDASM_EXPORT void rdsurface_seektoep(RDSurface* self);
