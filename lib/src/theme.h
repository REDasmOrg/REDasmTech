#pragma once

#include <redasm/theme.h>
#include <string>

namespace redasm {

struct Theme {
    std::string fg{"#000000"}, bg{"#ffffff"}, seek;
    std::string comment, autocomment;
    std::string highlightfg, highlightbg;
    std::string selectionfg, selectionbg;
    std::string cursorfg, cursorbg;
    std::string segment, function, type;
    std::string address, constant, reg;
    std::string string, symbol, data, pointer, imported;
    std::string nop, ret, call, jump, jumpcond;
    std::string entryfg, entrybg;
    std::string graphbg, graphedge, graphedgeloop, graphedgeloopcond;
    std::string success, fail, warning;

    [[nodiscard]] std::string_view get_color(RDThemeKind kind) const;
    bool set_color(RDThemeKind kind, const std::string& color);
};

} // namespace redasm
