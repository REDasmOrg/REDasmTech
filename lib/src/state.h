#pragma once

#include "theme.h"
#include <deque>
#include <functional>
#include <redasm/analyzer.h>
#include <redasm/loader.h>
#include <redasm/processor.h>
#include <string_view>
#include <vector>

namespace redasm {

class Context;
using ContextList = std::vector<Context*>;
using ErrorCallback = std::function<void(std::string_view)>;

namespace state {

inline Theme theme;                         // Current Theme
inline std::deque<RDLoader> loaders;        // Registered Loaders
inline std::vector<RDProcessor> processors; // Registered Processors
inline std::vector<RDAnalyzer> analyzers;   // Registered Analyzers
inline ContextList contextlist;             // Context Candidates
inline Context* context{};                  // Active Context
inline ErrorCallback onerror;               // Error callback

} // namespace state

} // namespace redasm
