#pragma once

#include <utility>

#if defined(__unix__)
#include "process_unix.h"
#else
#error "Process: Unsupported Operating System"
#endif

namespace redasm::process {

using Process = impl::Process;

std::pair<int, std::string>
run(std::string_view cmd, std::initializer_list<std::string_view> args = {});

[[nodiscard]]
int exec(std::string_view cmd,
         std::initializer_list<std::string_view> args = {});

} // namespace redasm::process
