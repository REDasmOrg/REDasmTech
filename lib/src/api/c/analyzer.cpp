#include "../internal/analyzer.h"
#include "../../plugins/origin.h"
#include <redasm/analyzer.h>

bool rd_registeranalyzer(const RDAnalyzerPlugin* plugin) {
    return redasm::api::internal::register_analyzer(plugin,
                                                    redasm::pm::Origin::NATIVE);
}

const RDAnalyzerPlugin** rd_getanalyzerplugins(usize* n) {
    return redasm::api::internal::get_analyzerplugins(n);
}

const RDAnalyzerPlugin** rd_getanalyzers(usize* n) {
    return redasm::api::internal::get_analyzers(n);
}

bool rdanalyzerplugin_select(const RDAnalyzerPlugin* self, bool select) {
    return redasm::api::internal::analyzerplugin_select(self, select);
}

bool rdanalyzerplugin_isselected(const RDAnalyzerPlugin* self) {
    return redasm::api::internal::analyzerplugin_isselected(self);
}
