#pragma once

#include <string>

namespace demangler {

std::string get_packagename(const std::string& s);
std::string get_objectname(const std::string& s);
std::string get_fullname(const std::string& s);
std::string get_signature(const std::string& s, bool wrap = true);
std::string get_return(const std::string& s);

} // namespace demangler
