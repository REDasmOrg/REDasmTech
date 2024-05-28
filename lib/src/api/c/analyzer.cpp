#include "../internal/analyzer.h"
#include <redasm/analyzer.h>

usize rd_getanalyzers(const RDAnalyzer** analyzers) {
    return redasm::api::internal::get_analyzers(analyzers);
}

bool rdanalyzer_select(const RDAnalyzer* self, bool select) {
    return self && redasm::api::internal::analyzer_select(self, select);
}

bool rdanalyzer_isselected(const RDAnalyzer* self) {
    return self && redasm::api::internal::analyzer_isselected(self);
}

void rd_analyze(void) { redasm::api::internal::analyze(); }
