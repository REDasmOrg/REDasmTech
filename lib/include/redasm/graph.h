#pragma once

#include "common.h"
#include "types.h"

RD_HANDLE(RDGraph);

typedef usize RDGraphNode;

typedef struct RDGraphEdge {
    RDGraphNode source, target;
} RDGraphEdge;

typedef struct RDGraphPoint {
    int x, y;
} RDGraphPoint;

typedef union RDGraphData {
    uptr nu_data;
    iptr ns_data;
    const char* s_data;
    void* p_data;
} RDGraphData;

// Graph
REDASM_EXPORT RDGraph* rdgraph_create(void);

REDASM_EXPORT const RDGraphEdge*
rdgraph_getedge(const RDGraph* self, RDGraphNode src, RDGraphNode tgt);

REDASM_EXPORT RDGraphNode rdgraph_pushnode(RDGraph* self);
REDASM_EXPORT RDGraphNode rdgraph_getroot(const RDGraph* self);

REDASM_EXPORT usize rdgraph_getoutgoing(const RDGraph* self, RDGraphNode n,
                                        const RDGraphEdge** edges);

REDASM_EXPORT usize rdgraph_getincoming(const RDGraph* self, RDGraphNode n,
                                        const RDGraphEdge** edges);

REDASM_EXPORT usize rdgraph_getnodes(const RDGraph* self,
                                     const RDGraphNode** nodes);

REDASM_EXPORT usize rdgraph_getedges(const RDGraph* self,
                                     const RDGraphEdge** edges);

REDASM_EXPORT bool rdgraph_containsedge(const RDGraph* self, RDGraphNode src,
                                        RDGraphNode tgt);

REDASM_EXPORT bool rdgraph_isempty(const RDGraph* self);

REDASM_EXPORT void rdgraph_pushedge(RDGraph* self, RDGraphNode src,
                                    RDGraphNode tgt);

REDASM_EXPORT void rdgraph_setroot(RDGraph* self, RDGraphNode n);
REDASM_EXPORT void rdgraph_removeedge(RDGraph* self, const RDGraphEdge* e);
REDASM_EXPORT void rdgraph_removenode(RDGraph* self, RDGraphNode n);
REDASM_EXPORT u32 rdgraph_hash(const RDGraph* self);
REDASM_EXPORT const char* rdgraph_generatedot(const RDGraph* self);

// Data
REDASM_EXPORT const RDGraphData* rdgraph_getdata(const RDGraph* self,
                                                 RDGraphNode n);

REDASM_EXPORT void rdgraph_setdatauint(RDGraph* self, RDGraphNode n, uptr val);
REDASM_EXPORT void rdgraph_setdataint(RDGraph* self, RDGraphNode n, iptr val);

REDASM_EXPORT void rdgraph_setdatastring(RDGraph* self, RDGraphNode n,
                                         const char* val);

REDASM_EXPORT void rdgraph_setdatapointer(RDGraph* self, RDGraphNode n,
                                          void* val);

// Styling
REDASM_EXPORT const char* rdgraph_getcolor(const RDGraph* self,
                                           const RDGraphEdge* e);

REDASM_EXPORT const char* rdgraph_getlabel(const RDGraph* self,
                                           const RDGraphEdge* e);

REDASM_EXPORT usize rdgraph_getroutes(const RDGraph* self, const RDGraphEdge* e,
                                      const RDGraphPoint** path);

REDASM_EXPORT usize rdgraph_getarrow(const RDGraph* self, const RDGraphEdge* e,
                                     const RDGraphPoint** path);

REDASM_EXPORT int rdgraph_getareawidth(const RDGraph* self);
REDASM_EXPORT int rdgraph_getareaheight(const RDGraph* self);
REDASM_EXPORT int rdgraph_getx(const RDGraph* self, RDGraphNode n);
REDASM_EXPORT int rdgraph_gety(const RDGraph* self, RDGraphNode n);
REDASM_EXPORT int rdgraph_getwidth(const RDGraph* self, RDGraphNode n);
REDASM_EXPORT int rdgraph_getheight(const RDGraph* self, RDGraphNode n);

REDASM_EXPORT void rdgraph_setcolor(RDGraph* self, const RDGraphEdge* e,
                                    const char* s);

REDASM_EXPORT void rdgraph_setlabel(RDGraph* self, const RDGraphEdge* e,
                                    const char* s);

REDASM_EXPORT void rdgraph_setroutes(RDGraph* self, const RDGraphEdge* e,
                                     const RDGraphPoint* poly, usize n);

REDASM_EXPORT void rdgraph_setarrow(RDGraph* self, const RDGraphEdge* e,
                                    const RDGraphPoint* poly, usize n);

REDASM_EXPORT void rdgraph_setareawidth(RDGraph* self, int w);
REDASM_EXPORT void rdgraph_setareaheight(RDGraph* self, int h);
REDASM_EXPORT void rdgraph_setx(RDGraph* self, RDGraphNode n, int x);
REDASM_EXPORT void rdgraph_sety(RDGraph* self, RDGraphNode n, int y);
REDASM_EXPORT void rdgraph_setwidth(RDGraph* self, RDGraphNode n, int w);
REDASM_EXPORT void rdgraph_setheight(RDGraph* self, RDGraphNode n, int h);
REDASM_EXPORT void rdgraph_clearlayout(RDGraph* self);

typedef enum RDLayeredLayoutType {
    LAYEREDLAYOUT_MEDIUM = 0,
    LAYEREDLAYOUT_NARROW,
    LAYEREDLAYOUT_WIDE,
} RDLayeredLayoutType;

REDASM_EXPORT bool rdgraphlayout_layered(RDGraph* self, usize type);

#ifdef __cplusplus
#include <functional>

namespace std {

template<>
struct hash<RDGraphEdge> {
    size_t operator()(const RDGraphEdge& edge) const {
        return edge.source ^ edge.target;
    }
};

template<>
struct equal_to<RDGraphEdge> {
    bool operator()(const RDGraphEdge& edge1, const RDGraphEdge& edge2) const {
        return (edge1.source == edge2.source) && (edge1.target == edge2.target);
    }
};

} // namespace std
#endif
