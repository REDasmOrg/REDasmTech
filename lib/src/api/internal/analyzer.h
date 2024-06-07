#pragma once

#include <redasm/analyzer.h>

namespace redasm::api::internal {

void register_analyzer(const RDAnalyzer& analyzer);
usize get_analyzers(const RDAnalyzer** analyzers);
bool analyzer_select(const RDAnalyzer* self, bool select);
bool analyzer_isselected(const RDAnalyzer* self);

} // namespace redasm::api::internal
