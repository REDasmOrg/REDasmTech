#pragma once

#include <redasm/analyzer.h>

namespace redasm::api::internal {

bool register_analyzer(const RDAnalyzerPlugin* plugin);
const RDAnalyzerPlugin** get_analyzerplugins(usize* n);
const RDAnalyzerPlugin** get_analyzers(usize* n);
bool analyzerplugin_select(const RDAnalyzerPlugin* self, bool select);
bool analyzerplugin_isselected(const RDAnalyzerPlugin* self);

} // namespace redasm::api::internal
