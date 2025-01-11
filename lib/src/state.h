#pragma once

#include "theme.h"
#include "typing/basetypes.h"
#include <redasm/redasm.h>
#include <string_view>

namespace redasm {

class Context;

namespace state {

using ContextList = std::vector<Context*>;

inline RDInitParams params;
inline Theme theme;
inline ContextList contextlist;
inline Context* context{};
inline const typing::BaseTypes BASETYPES;

inline void log(std::string_view s) {
    state::params.onlog(s.data(), state::params.userdata);
}

inline void status(std::string_view s) {
    state::params.onstatus(s.data(), state::params.userdata);
}

inline void error(std::string_view s) {
    state::params.onerror(s.data(), state::params.userdata);
}

const typing::BaseTypes& get_types();

} // namespace state

} // namespace redasm
