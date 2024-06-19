#include <redasm/graph.h>
#include <string>
#include <tl/optional.hpp>

namespace redasm::api::internal {

// Graph
RDGraph* graph_create();

const RDGraphEdge* graph_getedge(const RDGraph* self, RDGraphNode src,
                                 RDGraphNode tgt);

RDGraphNode graph_addnode(RDGraph* self);
RDGraphNode graph_getroot(const RDGraph* self);

usize graph_getoutgoing(const RDGraph* self, RDGraphNode n,
                        const RDGraphEdge** edges);

usize graph_getincoming(const RDGraph* self, RDGraphNode n,
                        const RDGraphEdge** edges);

usize graph_getnodes(const RDGraph* self, const RDGraphNode** nodes);
usize graph_getedges(const RDGraph* self, const RDGraphEdge** edges);
bool graph_containsedge(const RDGraph* self, RDGraphNode src, RDGraphNode tgt);
bool graph_isempty(const RDGraph* self);
void graph_addedge(RDGraph* self, RDGraphNode src, RDGraphNode tgt);

void graph_setroot(RDGraph* self, RDGraphNode n);
void graph_removeedge(RDGraph* self, const RDGraphEdge* e);
void graph_removenode(RDGraph* self, RDGraphNode n);
u32 graph_hash(const RDGraph* self);
std::string graph_generatedot(const RDGraph* self);

// Data
const RDGraphData* graph_getdata(const RDGraph* self, RDGraphNode n);
void graph_setdatauint(RDGraph* self, RDGraphNode n, uptr val);
void graph_setdataint(RDGraph* self, RDGraphNode n, iptr val);
void graph_setdatastring(RDGraph* self, RDGraphNode n, const char* val);
void graph_setdatapointer(RDGraph* self, RDGraphNode n, void* val);

// Styling
const char* graph_getcolor(const RDGraph* self, const RDGraphEdge* e);
const char* graph_getlabel(const RDGraph* self, const RDGraphEdge* e);

usize graph_getroutes(const RDGraph* self, const RDGraphEdge* e,
                      const RDGraphPoint** path);

usize graph_getarrow(const RDGraph* self, const RDGraphEdge* e,
                     const RDGraphPoint** path);

int graph_getareawidth(const RDGraph* self);
int graph_getareaheight(const RDGraph* self);
int graph_getx(const RDGraph* self, RDGraphNode n);
int graph_gety(const RDGraph* self, RDGraphNode n);
int graph_getwidth(const RDGraph* self, RDGraphNode n);
int graph_getheight(const RDGraph* self, RDGraphNode n);
void graph_setcolor(RDGraph* self, const RDGraphEdge* e, const std::string& s);
void graph_setlabel(RDGraph* self, const RDGraphEdge* e, const std::string& s);

void graph_setroutes(RDGraph* self, const RDGraphEdge* e,
                     const RDGraphPoint* poly, usize n);

void graph_setarrow(RDGraph* self, const RDGraphEdge* e,
                    const RDGraphPoint* poly, usize n);

void graph_setareawidth(RDGraph* self, int w);
void graph_setareaheight(RDGraph* self, int h);
void graph_setx(RDGraph* self, RDGraphNode n, int x);
void graph_sety(RDGraph* self, RDGraphNode n, int y);
void graph_setwidth(RDGraph* self, RDGraphNode n, int w);
void graph_setheight(RDGraph* self, RDGraphNode n, int h);
void graph_clearlayout(RDGraph* self);

bool graphlayout_layered(RDGraph* self, usize type);

} // namespace redasm::api::internal
