#include "../context.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include <redasm/analyzer.h>
#include <spdlog/spdlog.h>

bool rd_registeranalyzer(const RDAnalyzerPlugin* plugin) {
    spdlog::trace("rd_registeranalyzer({})", fmt::ptr(plugin));
    return redasm::pm::register_analyzer(plugin, redasm::pm::NATIVE);
}

bool rd_registeranalyzer_ex(const RDAnalyzerPlugin* plugin,
                            const char* origin) {
    spdlog::trace("rd_registeranalyzer_ex({}, '{}')", fmt::ptr(plugin), origin);
    return redasm::pm::register_analyzer(plugin, origin);
}

Vect(const RDAnalyzerPlugin*) rd_getanalyzerplugins() {
    spdlog::trace("rd_getanalyzerplugins()");
    return redasm::pm::analyzers;
}

const RDAnalyzerPlugin** rd_getanalyzers(usize* n) {
    spdlog::trace("rd_getanalyzers({})", fmt::ptr(n));

    if(!redasm::state::context) {
        if(n) *n = 0;
        return nullptr;
    }

    if(n) *n = redasm::state::context->analyzerplugins.size();
    return redasm::state::context->analyzerplugins.data();
}

bool rdanalyzerplugin_select(const RDAnalyzerPlugin* self, bool select) {
    spdlog::trace("rdanalyzerplugin_select({}, {})", fmt::ptr(self), select);

    redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    return std::ranges::any_of(ctx->analyzerplugins,
                               [&](const RDAnalyzerPlugin* x) {
                                   if(x != self) return false;

                                   if(select)
                                       ctx->selectedanalyzerplugins.insert(x);
                                   else
                                       ctx->selectedanalyzerplugins.erase(x);

                                   return true;
                               });
}

bool rdanalyzerplugin_isselected(const RDAnalyzerPlugin* self) {
    spdlog::trace("rdanalyzerplugin_isselected({})", fmt::ptr(self));
    return redasm::state::context &&
           redasm::state::context->selectedanalyzerplugins.contains(self);
}
