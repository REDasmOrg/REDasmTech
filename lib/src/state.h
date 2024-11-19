#pragma once

#include "theme.h"
#include <deque>
#include <redasm/analyzer.h>
#include <redasm/loader.h>
#include <redasm/processor.h>
#include <redasm/redasm.h>
#include <string_view>
#include <vector>

namespace redasm {

class Context;
using ContextList = std::vector<Context*>;

namespace state {

inline RDInitParams params;                 // Init Configuration
inline Theme theme;                         // Current Theme
inline std::deque<RDLoader> loaders;        // Registered Loaders
inline std::vector<RDProcessor> processors; // Registered Processors
inline std::vector<RDAnalyzer> analyzers;   // Registered Analyzers
inline ContextList contextlist;             // Context Candidates
inline Context* context{};                  // Active Context

inline void log(std::string_view s) {
    state::params.onlog(s.data(), state::params.userdata);
}

inline void status(std::string_view s) {
    state::params.onstatus(s.data(), state::params.userdata);
}

inline void error(std::string_view s) {
    state::params.onerror(s.data(), state::params.userdata);
}

} // namespace state

} // namespace redasm
