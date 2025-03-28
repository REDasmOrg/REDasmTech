#include "theme.h"

namespace redasm {

namespace {

std::string_view theme_alt(std::string_view color, std::string_view altcolor) {
    return color.empty() ? altcolor : color;
}

} // namespace

std::string_view Theme::get_color(RDThemeKind kind) const {
    switch(kind) {
        case THEME_FOREGROUND: return this->fg;
        case THEME_BACKGROUND: return this->bg;

        case THEME_SEEK: return theme_alt(this->seek, this->bg);
        case THEME_COMMENT: return theme_alt(this->comment, this->fg);

        case THEME_AUTOCOMMENT: return theme_alt(this->autocomment, this->fg);
        case THEME_HIGHLIGHTFG: return theme_alt(this->highlightfg, this->bg);
        case THEME_HIGHLIGHTBG: return theme_alt(this->highlightbg, this->fg);
        case THEME_SELECTIONFG: return theme_alt(this->selectionfg, this->bg);
        case THEME_SELECTIONBG: return theme_alt(this->selectionbg, this->fg);

        case THEME_CURSORFG: return theme_alt(this->cursorfg, this->bg);
        case THEME_CURSORBG: return theme_alt(this->cursorbg, this->fg);
        case THEME_SEGMENT: return theme_alt(this->segment, this->fg);
        case THEME_FUNCTION: return theme_alt(this->function, this->fg);
        case THEME_TYPE: return theme_alt(this->type, this->fg);
        case THEME_ADDRESS: return theme_alt(this->address, this->fg);
        case THEME_STRING: return theme_alt(this->string, this->fg);
        case THEME_LABEL: return theme_alt(this->symbol, this->fg);
        case THEME_DATA: return theme_alt(this->data, this->fg);
        case THEME_POINTER: return theme_alt(this->pointer, this->fg);
        case THEME_IMPORT: return theme_alt(this->imported, this->fg);
        case THEME_CONSTANT: return theme_alt(this->constant, this->fg);
        case THEME_REG: return theme_alt(this->reg, this->fg);
        case THEME_NOP: return theme_alt(this->nop, this->fg);
        case THEME_RET: return theme_alt(this->ret, this->fg);
        case THEME_CALL: return theme_alt(this->call, this->fg);
        case THEME_JUMP: return theme_alt(this->jump, this->fg);
        case THEME_JUMPCOND: return theme_alt(this->jumpcond, this->fg);
        case THEME_ENTRYFG: return theme_alt(this->entryfg, this->fg);
        case THEME_ENTRYBG: return theme_alt(this->entrybg, this->bg);
        case THEME_GRAPHBG: return theme_alt(this->graphbg, this->bg);
        case THEME_GRAPHEDGE: return theme_alt(this->graphedge, this->fg);

        case THEME_GRAPHEDGELOOP:
            return theme_alt(this->graphedgeloop, this->fg);
        case THEME_GRAPHEDGELOOPCOND:
            return theme_alt(this->graphedgeloopcond, this->fg);

        case THEME_SUCCESS: return theme_alt(this->success, this->fg);
        case THEME_FAIL: return theme_alt(this->fail, this->fg);
        case THEME_WARNING: return theme_alt(this->warning, this->fg);
        default: break;
    }

    return {};
}

bool Theme::set_color(RDThemeKind theme, const std::string& color) {
    switch(theme) {
        case THEME_FOREGROUND: this->fg = color; break;
        case THEME_BACKGROUND: this->bg = color; break;
        case THEME_SEEK: this->seek = color; break;
        case THEME_COMMENT: this->comment = color; break;
        case THEME_AUTOCOMMENT: this->autocomment = color; break;
        case THEME_HIGHLIGHTFG: this->highlightfg = color; break;
        case THEME_HIGHLIGHTBG: this->highlightbg = color; break;
        case THEME_SELECTIONFG: this->selectionfg = color; break;
        case THEME_SELECTIONBG: this->selectionbg = color; break;
        case THEME_CURSORFG: this->cursorfg = color; break;
        case THEME_CURSORBG: this->cursorbg = color; break;
        case THEME_SEGMENT: this->segment = color; break;
        case THEME_FUNCTION: this->function = color; break;
        case THEME_TYPE: this->type = color; break;
        case THEME_ADDRESS: this->address = color; break;
        case THEME_CONSTANT: this->constant = color; break;
        case THEME_REG: this->reg = color; break;
        case THEME_STRING: this->string = color; break;
        case THEME_LABEL: this->symbol = color; break;
        case THEME_DATA: this->data = color; break;
        case THEME_POINTER: this->pointer = color; break;
        case THEME_IMPORT: this->imported = color; break;
        case THEME_NOP: this->nop = color; break;
        case THEME_RET: this->ret = color; break;
        case THEME_CALL: this->call = color; break;
        case THEME_JUMP: this->jump = color; break;
        case THEME_JUMPCOND: this->jumpcond = color; break;
        case THEME_ENTRYFG: this->entryfg = color; break;
        case THEME_ENTRYBG: this->entrybg = color; break;
        case THEME_GRAPHBG: this->graphbg = color; break;
        case THEME_GRAPHEDGE: this->graphedge = color; break;
        case THEME_GRAPHEDGELOOP: this->graphedgeloop = color; break;
        case THEME_GRAPHEDGELOOPCOND: this->graphedgeloopcond = color; break;
        case THEME_SUCCESS: this->success = color; break;
        case THEME_FAIL: this->fail = color; break;
        case THEME_WARNING: this->warning = color; break;
        default: return false;
    }

    return true;
}

} // namespace redasm
