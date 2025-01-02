#include "demangler.h"
#include <regex>

namespace demangler {

namespace {

std::string get_type(const std::string& s) {
    if(s == "V") return "void";
    if(s == "Z") return "boolean";
    if(s == "B") return "byte";
    if(s == "S") return "short";
    if(s == "C") return "char";
    if(s == "I") return "int";
    if(s == "J") return "long";
    if(s == "F") return "float";
    if(s == "D") return "double";
    return s;
}

} // namespace

std::string get_packagename(const std::string& s) {
    std::regex rgx("L(.+)\\/.+;");
    std::smatch m;

    if(!std::regex_match(s, m, rgx)) return s;

    std::string sm = m[1];
    std::replace(sm.begin(), sm.end(), '/', '.');
    return sm;
}

std::string get_objectname(const std::string& s) {
    std::regex rgx("L(.+\\/)+(.+);");
    std::smatch m;

    return std::regex_match(s, m, rgx) ? m[2] : s;
}

std::string get_fullname(const std::string& s) {
    std::regex rgx("L(.+);");
    std::smatch m;

    if(!std::regex_match(s, m, rgx)) return s;

    std::string sm = m[1];
    std::replace(sm.begin(), sm.end(), '/', '.');
    return sm;
}

std::string get_signature(const std::string& s, bool wrap) {
    std::regex rgx("(L.+?;)");
    std::sregex_iterator it(s.begin(), s.end(), rgx);
    std::string res;

    for(; it != std::sregex_iterator(); it++) {
        if(!res.empty()) res += ", ";
        res += demangler::get_objectname((*it)[1]);
    }

    if(wrap) res = "(" + res + ")";
    return res;
}

std::string get_return(const std::string& s) {
    std::regex rgx("\\(.*\\)(.+)");
    std::smatch m;

    return std::regex_match(s, m, rgx) ? demangler::get_type(m[1]) : s;
}

} // namespace demangler
