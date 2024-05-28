#pragma once

#include <string>

namespace redasm {

void add_search_path(const std::string& p);
void unload_modules();
void load_modules();

} // namespace redasm
