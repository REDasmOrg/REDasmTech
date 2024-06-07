#include "analyzer.h"
#include "../../context.h"
#include "../../state.h"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void register_analyzer(const RDAnalyzer& analyzer) {
    spdlog::trace("register_analyzer('{}')", analyzer.name);

    auto it = std::upper_bound(
        state::analyzers.begin(), state::analyzers.end(), analyzer,
        [](const auto& a, const auto& b) { return a.order < b.order; });

    state::analyzers.insert(it, analyzer);
}

usize get_analyzers(const RDAnalyzer** analyzers) {
    spdlog::trace("get_analyzers({})", fmt::ptr(analyzers));

    if(analyzers)
        *analyzers = state::analyzers.data();

    return state::analyzers.size();
}

bool analyzer_select(const RDAnalyzer* self, bool select) {
    spdlog::trace("analyzer_select('{}', {})", self->name, select);

    return std::any_of(state::analyzers.begin(), state::analyzers.end(),
                       [&](const RDAnalyzer& x) {
                           if(&x != self)
                               return false;

                           if(select)
                               state::context->selectedanalyzers.insert(&x);
                           else
                               state::context->selectedanalyzers.erase(&x);

                           return true;
                       });
}

bool analyzer_isselected(const RDAnalyzer* self) {
    spdlog::trace("analyzer_isselected('{}')", self->name);

    if(!state::context)
        return false;

    const auto& analyzers = state::context->selectedanalyzers;
    return analyzers.find(self) != analyzers.end();
}

} // namespace redasm::api::internal
