#include "analyzer.h"
#include "../../context.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void register_analyzer(const RDAnalyzer& analyzer) {
    spdlog::trace("register_analyzer('{}')", analyzer.name);

    if(std::ranges::find_if(state::analyzers, [&](const RDAnalyzer& x) {
           return x.id == analyzer.id;
       }) != state::analyzers.end()) {
        state::error(fmt::format("Analyzer '{}' already exists", analyzer.id));
        return;
    }

    auto it = std::ranges::upper_bound(
        state::analyzers, analyzer,
        [](const auto& a, const auto& b) { return a.order < b.order; });

    state::analyzers.insert(it, analyzer);
}

usize get_analyzers(const RDAnalyzer** analyzers) {
    spdlog::trace("get_analyzers({})", fmt::ptr(analyzers));

    if(!state::context)
        return 0;

    if(analyzers)
        *analyzers = state::context->analyzers.data();

    return state::context->analyzers.size();
}

bool analyzer_select(const RDAnalyzer* self, bool select) {
    spdlog::trace("analyzer_select('{}', {})", self->name, select);

    Context* ctx = state::context;

    return std::ranges::any_of(ctx->analyzers, [&](const RDAnalyzer& x) {
        if(&x != self)
            return false;

        if(select)
            state::context->selectedanalyzers.insert(x.name);
        else
            state::context->selectedanalyzers.erase(x.name);

        return true;
    });
}

bool analyzer_isselected(const RDAnalyzer* self) {
    spdlog::trace("analyzer_isselected('{}')", self->name);

    if(!state::context)
        return false;

    return state::context->selectedanalyzers.contains(self->name);
}

} // namespace redasm::api::internal
