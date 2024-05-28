#pragma once

#include "../listing.h"
#include "../segment.h"
#include <redasm/surface.h>
#include <redasm/theme.h>
#include <string>
#include <string_view>
#include <vector>

namespace redasm {

using SurfaceRow = std::vector<RDSurfaceCell>;
using SurfaceRows = std::vector<SurfaceRow>;

struct Renderer {
    [[nodiscard]] usize current_address() const;
    void swap(SurfaceRows& r) { m_rows.swap(r); }
    void clear() { m_rows.clear(); }
    void highlight_row(usize row);
    void highlight_words(usize row, usize col);
    void highlight_selection(RDSurfacePosition startsel,
                             RDSurfacePosition endsel);
    void highlight_cursor(usize row, usize col);
    void fill_columns();
    void set_current_item(const ListingItem& item);

    Renderer& new_row(const ListingItem& item);

    Renderer& character(SurfaceRow& row, char ch,
                        RDThemeKind fg = THEME_DEFAULT,
                        RDThemeKind bg = THEME_DEFAULT);

    Renderer& chunk(std::string_view arg, RDThemeKind fg = THEME_DEFAULT,
                    RDThemeKind bg = THEME_DEFAULT);

    inline Renderer& arr_index(usize idx) {
        return this->chunk("[").constant(std::to_string(idx)).chunk("]");
    }

    inline Renderer& address(std::string_view arg) {
        return this->chunk(arg, THEME_ADDRESS);
    }

    inline Renderer& nop(std::string_view arg) {
        return this->chunk(arg, THEME_NOP);
    }

    inline Renderer& constant(std::string_view arg) {
        return this->chunk(arg, THEME_CONSTANT);
    }

    inline Renderer& string(std::string_view arg) {
        return this->chunk(arg, THEME_STRING);
    }

    inline Renderer& type(std::string_view arg) {
        return this->chunk(arg, THEME_TYPE);
    }

    inline Renderer& function(std::string_view arg) {
        return this->chunk(arg, THEME_FUNCTION);
    }

    inline Renderer& word(std::string_view arg) {
        return this->chunk(" ").chunk(arg).chunk(" ");
    }

    inline Renderer& quote(std::string_view arg, std::string_view q = "\"") {
        return this->chunk(q).chunk(arg).chunk(q);
    }

    inline Renderer& ws(usize n = 1) {
        return this->chunk(std::string(n, ' '));
    }

    static bool is_char_skippable(char ch);

public:
    usize columns{0};

private:
    const Segment* current_segment() const;
    void check_current_segment(const ListingItem& item);
    std::string word_at(usize row, usize col) const;

private:
    SurfaceRows m_rows;
    RDAddress m_curraddress{};
    usize m_currsegment{0};
};

} // namespace redasm
