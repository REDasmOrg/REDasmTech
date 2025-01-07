#include "analyzer.h"
#include "../../context.h"
#include "../../plugins/pluginmanager.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

bool register_analyzer(const RDAnalyzerPlugin* plugin) {
    spdlog::trace("register_analyzer({})", fmt::ptr(plugin));
    return pm::register_analyzer(plugin);
}

const RDAnalyzerPlugin** get_analyzerplugins(usize* n) {
    spdlog::trace("get_analyzersplugins({})", fmt::ptr(n));
    return pm::get_analyzers(n);
}

const RDAnalyzerPlugin** get_analyzers(usize* n) {
    spdlog::trace("get_analyzers({})", fmt::ptr(n));

    if(!state::context) {
        if(n) *n = 0;
        return nullptr;
    }

    if(n) *n = state::context->analyzerplugins.size();
    return state::context->analyzerplugins.data();
}

bool analyzerplugin_select(const RDAnalyzerPlugin* self, bool select) {
    spdlog::trace("analyzerplugin_select('{}', {})", self->name, select);

    Context* ctx = state::context;

    return std::ranges::any_of(
        ctx->analyzerplugins, [&](const RDAnalyzerPlugin* x) {
            if(x != self) return false;

            if(select)
                state::context->selectedanalyzerplugins.insert(x);
            else
                state::context->selectedanalyzerplugins.erase(x);

            return true;
        });
}

bool analyzerplugin_isselected(const RDAnalyzerPlugin* self) {
    spdlog::trace("analyzerplugin_isselected('{}')", self->name);
    if(!state::context) return false;
    return state::context->selectedanalyzerplugins.contains(self);
}

} // namespace redasm::api::internal
