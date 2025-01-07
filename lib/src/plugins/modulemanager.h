#pragma once

#include <string>
#include <vector>

namespace redasm::mm {

using SearchPaths = std::vector<std::string>;

const SearchPaths& get_searchpaths();
void add_searchpath(const std::string& sp);
void unload_all();
void load_all();

} // namespace redasm::mm
