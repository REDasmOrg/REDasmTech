#pragma once

#include <redasm/common.h>
#include <redasm/function.h>
#include <redasm/theme.h>
#include <redasm/types.h>

RD_HANDLE(RDSurface);

typedef enum RDSurfaceFlags {
    SURFACE_DEFAULT = 0u,
    SURFACE_NOADDRESS = 1u << 0,
    SURFACE_NOSEGMENT = 1u << 1,
    SURFACE_NOFUNCTION = 1u << 2,
    SURFACE_NOREFS = 1u << 3,
    SURFACE_NOCOMMENTS = 1u << 4,
    SURFACE_NOCURSOR = 1u << 5,
    SURFACE_NOCURSORLINE = 1u << 6,
    SURFACE_NOSELECTION = 1u << 7,
    SURFACE_NOHIGHLIGHT = 1u << 8,

    SURFACE_GRAPH = SURFACE_NOADDRESS | SURFACE_NOSEGMENT | SURFACE_NOFUNCTION |
                    SURFACE_NOCOMMENTS,
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
    } cursoraddress;

    struct {
        RDAddress value;
        bool valid;
    } address;

    struct {
        RDOffset value;
        bool valid;
    } offset;

    struct {
        MIndex value;
        bool valid;
    } index;

    struct {
        LIndex value;
        bool valid;
    } startindex;

    struct {
        int type;
        LIndex value;
        bool valid;
    } listingindex;

    const char* segment;
} RDSurfaceLocation;

typedef struct RDSurfacePosition {
    int row;
    int col;
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
REDASM_EXPORT void rdsurface_render(RDSurface* self, usize n);
REDASM_EXPORT void rdsurface_renderfunction(RDSurface* self,
                                            const RDFunction* f);
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
REDASM_EXPORT bool rdsurface_getindex(const RDSurface* self, MIndex* index);
REDASM_EXPORT int rdsurface_indexof(const RDSurface* self, RDAddress address);
REDASM_EXPORT int rdsurface_lastindexof(const RDSurface* self,
                                        RDAddress address);
REDASM_EXPORT const char* rdsurface_gettext(const RDSurface* self);
REDASM_EXPORT const char* rdsurface_getselectedtext(const RDSurface* self);
REDASM_EXPORT bool rdsurface_getaddressundercursor(const RDSurface* self,
                                                   RDAddress* address);
REDASM_EXPORT void rdsurface_setcolumns(RDSurface* self, usize columns);
REDASM_EXPORT void rdsurface_setposition(RDSurface* self, int row, int col);
REDASM_EXPORT bool rdsurface_selectword(RDSurface* self, int row, int col);
REDASM_EXPORT bool rdsurface_select(RDSurface* self, int row, int col);
REDASM_EXPORT void rdsurface_clearselection(RDSurface* self);
REDASM_EXPORT void rdsurface_clearhistory(RDSurface* self);
REDASM_EXPORT void rdsurface_seekposition(RDSurface* self, LIndex index);
REDASM_EXPORT void rdsurface_seek(RDSurface* self, LIndex index);
REDASM_EXPORT bool rdsurface_jumptoep(RDSurface* self);
REDASM_EXPORT bool rdsurface_cangoback(const RDSurface* self);
REDASM_EXPORT bool rdsurface_cangoforward(const RDSurface* self);
REDASM_EXPORT bool rdsurface_goback(RDSurface* self);
REDASM_EXPORT bool rdsurface_goforward(RDSurface* self);
