#pragma once

#include <string>
#include <vector>

namespace redasm {

using SearchPaths = std::vector<std::string>;

const SearchPaths& get_searchpaths();
void add_searchpath(const std::string& sp);
void unload_modules();
void load_modules();

} // namespace redasm
