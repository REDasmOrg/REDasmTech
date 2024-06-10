#include "../internal/graph.h"
#include <redasm/graph.h>

RDGraph* rdgraph_create() { return redasm::api::internal::graph_create(); }

const RDGraphEdge* rdgraph_getedge(const RDGraph* self, RDGraphNode src,
                                   RDGraphNode tgt) {
    return redasm::api::internal::graph_getedge(self, src, tgt);
}

RDGraphNode rdgraph_pushnode(RDGraph* self) {
    return redasm::api::internal::graph_pushnode(self);
}

RDGraphNode rdgraph_getroot(const RDGraph* self) {
    return redasm::api::internal::graph_getroot(self);
}

usize rdgraph_getoutgoing(const RDGraph* self, RDGraphNode n,
                          const RDGraphEdge** edges) {
    return redasm::api::internal::graph_getoutgoing(self, n, edges);
}

usize rdgraph_getincoming(const RDGraph* self, RDGraphNode n,
                          const RDGraphEdge** edges) {
    return redasm::api::internal::graph_getincoming(self, n, edges);
}

usize rdgraph_getnodes(const RDGraph* self, const RDGraphNode** nodes) {
    return redasm::api::internal::graph_getnodes(self, nodes);
}

usize rdgraph_getedges(const RDGraph* self, const RDGraphEdge** edges) {
    return redasm::api::internal::graph_getedges(self, edges);
}

bool rdgraph_containsedge(const RDGraph* self, RDGraphNode src,
                          RDGraphNode tgt) {
    return redasm::api::internal::graph_containsedge(self, src, tgt);
}

bool rdgraph_isempty(const RDGraph* self) {
    return redasm::api::internal::graph_isempty(self);
}

void rdgraph_pushedge(RDGraph* self, RDGraphNode src, RDGraphNode tgt) {
    redasm::api::internal::graph_pushedge(self, src, tgt);
}

void rdgraph_setroot(RDGraph* self, RDGraphNode n) {
    redasm::api::internal::graph_setroot(self, n);
}

void rdgraph_removeedge(RDGraph* self, const RDGraphEdge* e) {
    redasm::api::internal::graph_removeedge(self, e);
}

void rdgraph_removenode(RDGraph* self, RDGraphNode n) {
    redasm::api::internal::graph_removenode(self, n);
}

u32 rdgraph_hash(const RDGraph* self) {
    return redasm::api::internal::graph_hash(self);
}

const char* rdgraph_generatedot(const RDGraph* self) {
    static std::string s;
    s = redasm::api::internal::graph_generatedot(self);
    return s.c_str();
}

const RDGraphData* rdgraph_getdata(const RDGraph* self, RDGraphNode n) {
    return redasm::api::internal::graph_getdata(self, n);
}

void rdgraph_setdatauint(RDGraph* self, RDGraphNode n, uptr val) {
    redasm::api::internal::graph_setdatauint(self, n, val);
}

void rdgraph_setdataint(RDGraph* self, RDGraphNode n, iptr val) {
    redasm::api::internal::graph_setdataint(self, n, val);
}

void rdgraph_setdatastring(RDGraph* self, RDGraphNode n, const char* val) {
    redasm::api::internal::graph_setdatastring(self, n, val);
}

void rdgraph_setdatapointer(RDGraph* self, RDGraphNode n, void* val) {
    redasm::api::internal::graph_setdatapointer(self, n, val);
}

const char* rdgraph_getcolor(const RDGraph* self, const RDGraphEdge* e) {
    return redasm::api::internal::graph_getcolor(self, e);
}

const char* rdgraph_getlabel(const RDGraph* self, const RDGraphEdge* e) {
    return redasm::api::internal::graph_getlabel(self, e);
}

usize rdgraph_getroutes(const RDGraph* self, const RDGraphEdge* e,
                        const RDGraphPoint** path) {
    return redasm::api::internal::graph_getroutes(self, e, path);
}

usize rdgraph_getarrow(const RDGraph* self, const RDGraphEdge* e,
                       const RDGraphPoint** path) {
    return redasm::api::internal::graph_getarrow(self, e, path);
}

int rdgraph_getareawidth(const RDGraph* self) {
    return redasm::api::internal::graph_getareawidth(self);
}

int rdgraph_getareaheight(const RDGraph* self) {
    return redasm::api::internal::graph_getareaheight(self);
}

int rdgraph_getx(const RDGraph* self, RDGraphNode n) {
    return redasm::api::internal::graph_getx(self, n);
}

int rdgraph_gety(const RDGraph* self, RDGraphNode n) {
    return redasm::api::internal::graph_gety(self, n);
}

int rdgraph_getwidth(const RDGraph* self, RDGraphNode n) {
    return redasm::api::internal::graph_getwidth(self, n);
}

int rdgraph_getheight(const RDGraph* self, RDGraphNode n) {
    return redasm::api::internal::graph_getheight(self, n);
}

void rdgraph_setcolor(RDGraph* self, const RDGraphEdge* e, const char* s) {
    if(s)
        redasm::api::internal::graph_setcolor(self, e, s);
    else
        redasm::api::internal::graph_setcolor(self, e, {});
}

void rdgraph_setlabel(RDGraph* self, const RDGraphEdge* e, const char* s) {
    if(s)
        redasm::api::internal::graph_setlabel(self, e, s);
    else
        redasm::api::internal::graph_setlabel(self, e, {});
}

void rdgraph_setroutes(RDGraph* self, const RDGraphEdge* e,
                       const RDGraphPoint* poly, usize n) {
    redasm::api::internal::graph_setroutes(self, e, poly, n);
}

void rdgraph_setarrow(RDGraph* self, const RDGraphEdge* e,
                      const RDGraphPoint* poly, usize n) {
    redasm::api::internal::graph_setarrow(self, e, poly, n);
}

void rdgraph_setareawidth(RDGraph* self, int w) {
    redasm::api::internal::graph_setareawidth(self, w);
}

void rdgraph_setareaheight(RDGraph* self, int h) {
    redasm::api::internal::graph_setareaheight(self, h);
}

void rdgraph_setx(RDGraph* self, RDGraphNode n, int x) {
    redasm::api::internal::graph_setx(self, n, x);
}

void rdgraph_sety(RDGraph* self, RDGraphNode n, int y) {
    redasm::api::internal::graph_setx(self, n, y);
}

void rdgraph_setwidth(RDGraph* self, RDGraphNode n, int w) {
    redasm::api::internal::graph_setwidth(self, n, w);
}

void rdgraph_setheight(RDGraph* self, RDGraphNode n, int h) {
    redasm::api::internal::graph_setheight(self, n, h);
}

void rdgraph_clearlayout(RDGraph* self) {
    redasm::api::internal::graph_clearlayout(self);
}

bool rdgraphlayout_layered(RDGraph* self, usize type) {
    return redasm::api::internal::graphlayout_layered(self, type);
}
