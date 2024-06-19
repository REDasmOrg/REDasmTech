#include "graph.h"
#include "../../graph/layouts/layeredlayout.h"
#include "../../graph/styledgraph.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

RDGraph* graph_create() {
    spdlog::trace("graph_create()");
    return api::to_c(new StyledGraph());
}

const RDGraphEdge* graph_getedge(const RDGraph* self, RDGraphNode src,
                                 RDGraphNode tgt) {
    spdlog::trace("graph_getedge({}, {}, {})", fmt::ptr(self), src, tgt);
    return api::from_c(self)->edge(src, tgt);
}

RDGraphNode graph_addnode(RDGraph* self) {
    spdlog::trace("graph_addnode({})", fmt::ptr(self));
    return api::from_c(self)->add_node();
}

RDGraphNode graph_getroot(const RDGraph* self) {
    spdlog::trace("graph_getroot({})", fmt::ptr(self));
    return api::from_c(self)->root();
}

usize graph_getoutgoing(const RDGraph* self, RDGraphNode n,
                        const RDGraphEdge** edges) {
    spdlog::trace("graph_getoutgoing({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(edges));
    return api::from_c(self)->outgoing(n, edges);
}

usize graph_getincoming(const RDGraph* self, RDGraphNode n,
                        const RDGraphEdge** edges) {
    spdlog::trace("graph_getincoming({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(edges));
    return api::from_c(self)->incoming(n, edges);
}

usize graph_getnodes(const RDGraph* self, const RDGraphNode** nodes) {
    spdlog::trace("graph_getnodes({}, {})", fmt::ptr(self), fmt::ptr(nodes));
    return api::from_c(self)->nodes(nodes);
}

usize graph_getedges(const RDGraph* self, const RDGraphEdge** edges) {
    spdlog::trace("graph_getedges({}, {})", fmt::ptr(self), fmt::ptr(edges));
    return api::from_c(self)->edges(edges);
}

bool graph_containsedge(const RDGraph* self, RDGraphNode src, RDGraphNode tgt) {
    spdlog::trace("graph_containsedge({}, {}, {})", fmt::ptr(self), src, tgt);
    return api::from_c(self)->contains_edge(src, tgt);
}

bool graph_isempty(const RDGraph* self) {
    spdlog::trace("graph_isempty({})", fmt::ptr(self));
    return api::from_c(self)->empty();
}

void graph_addedge(RDGraph* self, RDGraphNode src, RDGraphNode tgt) {
    spdlog::trace("graph_addedge({}, {}, {})", fmt::ptr(self), src, tgt);
    api::from_c(self)->add_edge(src, tgt);
}

void graph_setroot(RDGraph* self, RDGraphNode n) {
    spdlog::trace("graph_setroot({}, {})", fmt::ptr(self), n);
    api::from_c(self)->set_root(n);
}

void graph_removeedge(RDGraph* self, const RDGraphEdge* e) {
    spdlog::trace("graph_removeedge({}, {})", fmt::ptr(self), fmt::ptr(e));
    if(e)
        api::from_c(self)->remove_edge(e);
}

void graph_removenode(RDGraph* self, RDGraphNode n) {
    spdlog::trace("graph_removenode({}, {})", fmt::ptr(self), n);
    api::from_c(self)->remove_node(n);
}

u32 graph_hash(const RDGraph* self) {
    spdlog::trace("graph_hash({})", fmt::ptr(self));
    return api::from_c(self)->hash();
}

std::string graph_generatedot(const RDGraph* self) {
    spdlog::trace("graph_generatedot({})", fmt::ptr(self));
    return api::from_c(self)->generate_dot();
}

const RDGraphData* graph_getdata(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("graph_getdata({}, {})", fmt::ptr(self), n);
    return api::from_c(self)->data(n);
}

void graph_setdatauint(RDGraph* self, RDGraphNode n, uptr val) {
    spdlog::trace("graph_setdatauint({}, {}, {})", fmt::ptr(self), n, val);
    api::from_c(self)->set_data(n, val);
}

void graph_setdataint(RDGraph* self, RDGraphNode n, iptr val) {
    spdlog::trace("graph_setdataint({}, {}, {})", fmt::ptr(self), n, val);
    api::from_c(self)->set_data(n, val);
}

void graph_setdatastring(RDGraph* self, RDGraphNode n, const char* val) {
    spdlog::trace("graph_setdatastring({}, {}, {})", fmt::ptr(self), n, val);
    api::from_c(self)->set_data(n, val);
}

void graph_setdatapointer(RDGraph* self, RDGraphNode n, void* val) {
    spdlog::trace("graph_setdatapointer({}, {}, {})", fmt::ptr(self), n, val);
    api::from_c(self)->set_data(n, val);
}

const char* graph_getcolor(const RDGraph* self, const RDGraphEdge* e) {
    spdlog::trace("graph_getcolor({}, {})", fmt::ptr(self), fmt::ptr(e));
    if(e)
        return api::from_c(self)->color(e).data();
    return nullptr;
}

const char* graph_getlabel(const RDGraph* self, const RDGraphEdge* e) {
    spdlog::trace("graph_getlabel({}, {})", fmt::ptr(self), fmt::ptr(e));
    if(e)
        return api::from_c(self)->label(e).data();
    return nullptr;
}

usize graph_getroutes(const RDGraph* self, const RDGraphEdge* e,
                      const RDGraphPoint** path) {
    spdlog::trace("graph_getroutes({}, {}, {})", fmt::ptr(self), fmt::ptr(e),
                  fmt::ptr(path));
    if(e)
        return api::from_c(self)->routes(e, path);
    return 0;
}

usize graph_getarrow(const RDGraph* self, const RDGraphEdge* e,
                     const RDGraphPoint** path) {
    spdlog::trace("graph_getarrow({}, {}, {})", fmt::ptr(self), fmt::ptr(e),
                  fmt::ptr(path));
    if(e)
        return api::from_c(self)->arrow(e, path);
    return 0;
}

int graph_getareawidth(const RDGraph* self) {
    spdlog::trace("graph_getareawidth({})", fmt::ptr(self));
    return api::from_c(self)->area_width();
}

int graph_getareaheight(const RDGraph* self) {
    spdlog::trace("graph_getareaheight({})", fmt::ptr(self));
    return api::from_c(self)->area_height();
}

int graph_getx(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("graph_getx({}, {})", fmt::ptr(self), n);
    return api::from_c(self)->x(n);
}

int graph_gety(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("graph_gety({}, {})", fmt::ptr(self), n);
    return api::from_c(self)->y(n);
}

int graph_getwidth(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("graph_getwidth({}, {})", fmt::ptr(self), n);
    return api::from_c(self)->width(n);
}

int graph_getheight(const RDGraph* self, RDGraphNode n) {
    spdlog::trace("graph_getheight({}, {})", fmt::ptr(self), n);
    return api::from_c(self)->height(n);
}

void graph_setcolor(RDGraph* self, const RDGraphEdge* e, const std::string& s) {
    spdlog::trace("graph_setcolor({}, {}, '{}')", fmt::ptr(self), fmt::ptr(e),
                  s);
    if(!e)
        return;

    api::from_c(self)->set_color(e, s);
}

void graph_setlabel(RDGraph* self, const RDGraphEdge* e, const std::string& s) {
    spdlog::trace("graph_setlabel({}, {}, '{}')", fmt::ptr(self), fmt::ptr(e),
                  s);

    if(!e)
        return;

    api::from_c(self)->set_label(e, s);
}

void graph_setroutes(RDGraph* self, const RDGraphEdge* e,
                     const RDGraphPoint* poly, usize n) {
    spdlog::trace("graph_setroutes({}, {}, {}, {})", fmt::ptr(self),
                  fmt::ptr(e), fmt::ptr(poly), n);

    if(!e || !poly)
        return;

    api::from_c(self)->set_routes(e, poly, n);
}

void graph_setarrow(RDGraph* self, const RDGraphEdge* e,
                    const RDGraphPoint* poly, usize n) {
    spdlog::trace("graph_setarrow({}, {}, {}, {})", fmt::ptr(self), fmt::ptr(e),
                  fmt::ptr(poly), n);

    if(!e || !poly)
        return;

    api::from_c(self)->set_arrow(e, poly, n);
}

void graph_setareawidth(RDGraph* self, int w) {
    spdlog::trace("graph_setareawidth({}, {})", fmt::ptr(self), w);
    api::from_c(self)->set_areawidth(w);
}

void graph_setareaheight(RDGraph* self, int h) {
    spdlog::trace("graph_setareaheight({}, {})", fmt::ptr(self), h);
    api::from_c(self)->set_areaheight(h);
}

void graph_setx(RDGraph* self, RDGraphNode n, int x) {
    spdlog::trace("graph_setx({}, {}, {})", fmt::ptr(self), n, x);
    api::from_c(self)->set_x(n, x);
}

void graph_sety(RDGraph* self, RDGraphNode n, int y) {
    spdlog::trace("graph_sety({}, {}, {})", fmt::ptr(self), n, y);
    api::from_c(self)->set_y(n, y);
}

void graph_setwidth(RDGraph* self, RDGraphNode n, int w) {
    spdlog::trace("graph_setwidth({}, {}, {})", fmt::ptr(self), n, w);
    api::from_c(self)->set_width(n, w);
}

void graph_setheight(RDGraph* self, RDGraphNode n, int h) {
    spdlog::trace("graph_setheight({}, {}, {})", fmt::ptr(self), n, h);
    api::from_c(self)->set_height(n, h);
}

void graph_clearlayout(RDGraph* self) {
    spdlog::trace("graph_clearlayout({})");
    api::from_c(self)->clear_layout();
}

bool graphlayout_layered(RDGraph* self, usize type) {
    spdlog::trace("graphlayout_layered({}, {})", fmt::ptr(self), type);

    LayeredLayout ll(api::from_c(self), type);
    return ll.execute();
}

} // namespace redasm::api::internal
