#pragma once

#include "theme.h"
#include "typing/typing.h"
#include <redasm/redasm.h>
#include <string_view>

namespace redasm {

class Context;

namespace state {

inline RDTestResultSlice tests;
inline bool initialized = false;
inline RDInitParams params;
inline Theme theme;
inline Context* context{nullptr};
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
