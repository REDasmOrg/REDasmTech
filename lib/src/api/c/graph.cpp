#include "../../graph/layouts/layeredlayout.h"
#include "../../graph/styledgraph.h"
#include "../marshal.h"
#include <redasm/graph.h>
#include <spdlog/spdlog.h>

RDGraph* rdgraph_create() {
    spdlog::trace("rdgraph_create()");
    return redasm::api::to_c(new redasm::StyledGraph{});
}

const RDGraphEdge* rdgraph_getedge(const RDGraph* self, RDGraphNode src,
                                   RDGraphNode tgt) {
    spdlog::trace("rdgraph_getedge({}, {}, {})", fmt::ptr(self), src, tgt);
    return redasm::api::from_c(self)->edge(src, tgt);
}

RDGraphNode rdgraph_addnode(RDGraph* self) {
    spdlog::trace("rdgraph_addnode({})", fmt::ptr(self));
    return redasm::api::from_c(self)->add_node();
}

RDGraphNode rdgraph_getroot(const RDGraph* self) {
    spdlog::trace("rdgraph_getroot({})", fmt::ptr(self));
    return redasm::api::from_c(self)->root();
}

usize rdgraph_getoutgoing(const RDGraph* self, RDGraphNode n,
                          const RDGraphEdge** edges) {
    spdlog::trace("rdgraph_getoutgoing({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(edges));
    return redasm::api::from_c(self)->outgoing(n, edges);
}

usize rdgraph_getincoming(const RDGraph* self, RDGraphNode n,
                          const RDGraphEdge** edges) {
    spdlog::trace("rdgraph_getincoming({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(edges));
    return redasm::api::from_c(self)->incoming(n, edges);
}

usize rdgraph_getnodes(const RDGraph* self, const RDGraphNode** nodes) {
    spdlog::trace("rdgraph_getnodes({}, {})", fmt::ptr(self), fmt::ptr(nodes));
    return redasm::api::from_c(self)->nodes(nodes);
}

usize rdgraph_getedges(const RDGraph* self, const RDGraphEdge** edges) {
    spdlog::trace("rdgraph_getedges({}, {})", fmt::ptr(self), fmt::ptr(edges));
    return redasm::api::from_c(self)->edges(edges);
}

bool rdgraph_containsedge(const RDGraph* self, RDGraphNode src,
                          RDGraphNode tgt) {
    spdlog::trace("rdgraph_containsedge({}, {}, {})", fmt::ptr(self), src, tgt);
    return redasm::api::from_c(self)->contains_edge(src, tgt);
}

bool rdgraph_isempty(const RDGraph* self) {
    spdlog::trace("rdgraph_isempty({})", fmt::ptr(self));
    return redasm::api::from_c(self)->empty();
}

void rdgraph_addedge(RDGraph* self, RDGraphNode src, RDGraphNode tgt) {
    spdlog::trace("rdgraph_addedge({}, {}, {})", fmt::ptr(self), src, tgt);
    redasm::api::from_c(self)->add_edge(src, tgt);
}

void rdgraph_setroot(RDGraph* self, RDGraphNode n) {
    spdlog::trace("rdgraph_setroot({}, {})", fmt::ptr(self), n);
    redasm::api::from_c(self)->set_root(n);
}

void rdgraph_removeedge(RDGraph* self, const RDGraphEdge* e) {
    spdlog::trace("rdgraph_removeedge({}, {})", fmt::ptr(self), fmt::ptr(e));
    if(e) redasm::api::from_c(self)->remove_edge(e);
}

void rdgraph_removenode(RDGraph* self, RDGraphNode n) {
    spdlog::trace("rdgraph_removenode({}, {})", fmt::ptr(self), n);
    redasm::api::from_c(self)->remove_node(n);
}

u32 rdgraph_hash(const RDGraph* self) {
    spdlog::trace("rdgraph_hash({})", fmt::ptr(self));
    return redasm::api::from_c(self)->hash();
}

const char* rdgraph_generatedot(const RDGraph* self) {
    spdlog::trace("rdgraph_generatedot({})", fmt::ptr(self));
    static std::string s;
    s = redasm::api::from_c(self)->generate_dot();
    return s.c_str();
}

const RDGraphData* rdgraph_getdata(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("rdgraph_getdata({}, {})", fmt::ptr(self), n);
    return redasm::api::from_c(self)->data(n);
}

void rdgraph_setdatauint(RDGraph* self, RDGraphNode n, uptr val) {
    spdlog::trace("rdgraph_setdatauint({}, {}, {})", fmt::ptr(self), n, val);
    redasm::api::from_c(self)->set_data(n, val);
}

void rdgraph_setdataint(RDGraph* self, RDGraphNode n, iptr val) {
    spdlog::trace("rdgraph_setdataint({}, {}, {})", fmt::ptr(self), n, val);
    redasm::api::from_c(self)->set_data(n, val);
}

void rdgraph_setdatastring(RDGraph* self, RDGraphNode n, const char* val) {
    spdlog::trace("rdgraph_setdatastring({}, {}, {})", fmt::ptr(self), n, val);
    redasm::api::from_c(self)->set_data(n, val);
}

void rdgraph_setdatapointer(RDGraph* self, RDGraphNode n, void* val) {
    spdlog::trace("rdgraph_setdatapointer({}, {}, {})", fmt::ptr(self), n, val);
    redasm::api::from_c(self)->set_data(n, val);
}

const char* rdgraph_getcolor(const RDGraph* self, const RDGraphEdge* e) {
    spdlog::trace("rdgraph_getcolor({}, {})", fmt::ptr(self), fmt::ptr(e));
    if(e) return redasm::api::from_c(self)->color(e).data();
    return nullptr;
}

const char* rdgraph_getlabel(const RDGraph* self, const RDGraphEdge* e) {
    spdlog::trace("rdgraph_getlabel({}, {})", fmt::ptr(self), fmt::ptr(e));
    if(e) return redasm::api::from_c(self)->label(e).data();
    return nullptr;
}

usize rdgraph_getroutes(const RDGraph* self, const RDGraphEdge* e,
                        const RDGraphPoint** path) {
    spdlog::trace("rdgraph_getroutes({}, {}, {})", fmt::ptr(self), fmt::ptr(e),
                  fmt::ptr(path));
    if(e) return redasm::api::from_c(self)->routes(e, path);
    return 0;
}

usize rdgraph_getarrow(const RDGraph* self, const RDGraphEdge* e,
                       const RDGraphPoint** path) {
    spdlog::trace("rdgraph_getarrow({}, {}, {})", fmt::ptr(self), fmt::ptr(e),
                  fmt::ptr(path));
    if(e) return redasm::api::from_c(self)->arrow(e, path);
    return 0;
}

int rdgraph_getareawidth(const RDGraph* self) {
    spdlog::trace("rdgraph_getareawidth({})", fmt::ptr(self));
    return redasm::api::from_c(self)->area_width();
}

int rdgraph_getareaheight(const RDGraph* self) {
    spdlog::trace("rdgraph_getareaheight({})", fmt::ptr(self));
    return redasm::api::from_c(self)->area_height();
}

int rdgraph_getx(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("rdgraph_getx({}, {})", fmt::ptr(self), n);
    return redasm::api::from_c(self)->x(n);
}

int rdgraph_gety(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("rdgraph_gety({}, {})", fmt::ptr(self), n);
    return redasm::api::from_c(self)->y(n);
}

int rdgraph_getwidth(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("rdgraph_getwidth({}, {})", fmt::ptr(self), n);
    return redasm::api::from_c(self)->width(n);
}

int rdgraph_getheight(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("rdgraph_getheight({}, {})", fmt::ptr(self), n);
    return redasm::api::from_c(self)->height(n);
}

void rdgraph_setcolor(RDGraph* self, const RDGraphEdge* e, const char* s) {
    spdlog::trace("rdgraph_setcolor({}, {}, '{}')", fmt::ptr(self), fmt::ptr(e),
                  s);
    if(e) redasm::api::from_c(self)->set_color(e, s ? s : std::string{});
}

void rdgraph_setlabel(RDGraph* self, const RDGraphEdge* e, const char* s) {
    spdlog::trace("rdgraph_setlabel({}, {}, '{}')", fmt::ptr(self), fmt::ptr(e),
                  s);

    if(e) redasm::api::from_c(self)->set_label(e, s ? s : std::string{});
}

void rdgraph_setroutes(RDGraph* self, const RDGraphEdge* e,
                       const RDGraphPoint* poly, usize n) {
    spdlog::trace("rddraph_setroutes({}, {}, {}, {})", fmt::ptr(self),
                  fmt::ptr(e), fmt::ptr(poly), n);

    if(e && poly) redasm::api::from_c(self)->set_routes(e, poly, n);
}

void rdgraph_setarrow(RDGraph* self, const RDGraphEdge* e,
                      const RDGraphPoint* poly, usize n) {
    spdlog::trace("rdgraph_setarrow({}, {}, {}, {})", fmt::ptr(self),
                  fmt::ptr(e), fmt::ptr(poly), n);

    if(e && poly) redasm::api::from_c(self)->set_arrow(e, poly, n);
}

void rdgraph_setareawidth(RDGraph* self, int w) {
    spdlog::trace("rdgraph_setareawidth({}, {})", fmt::ptr(self), w);
    redasm::api::from_c(self)->set_areawidth(w);
}

void rdgraph_setareaheight(RDGraph* self, int h) {
    spdlog::trace("rdgraph_setareawidth({}, {})", fmt::ptr(self), h);
    redasm::api::from_c(self)->set_areaheight(h);
}

void rdgraph_setx(RDGraph* self, RDGraphNode n, int x) {
    spdlog::trace("rdgraph_setx({}, {}, {})", fmt::ptr(self), n, x);
    redasm::api::from_c(self)->set_x(n, x);
}

void rdgraph_sety(RDGraph* self, RDGraphNode n, int y) {
    spdlog::trace("rdgraph_sety({}, {}, {})", fmt::ptr(self), n, y);
    redasm::api::from_c(self)->set_y(n, y);
}

void rdgraph_setwidth(RDGraph* self, RDGraphNode n, int w) {
    spdlog::trace("rdgraph_setwidth({}, {}, {})", fmt::ptr(self), n, w);
    redasm::api::from_c(self)->set_width(n, w);
}

void rdgraph_setheight(RDGraph* self, RDGraphNode n, int h) {
    spdlog::trace("rdgraph_setheight({}, {}, {})", fmt::ptr(self), n, h);
    redasm::api::from_c(self)->set_width(n, h);
}

void rdgraph_clearlayout(RDGraph* self) {
    spdlog::trace("rdgraph_clearlayout({})", fmt::ptr(self));
    redasm::api::from_c(self)->clear_layout();
}

bool rdgraphlayout_layered(RDGraph* self, usize type) {
    spdlog::trace("rdgraphlayout_layered({}, {})", fmt::ptr(self), type);
    redasm::LayeredLayout ll(redasm::api::from_c(self), type);
    return ll.execute();
}
