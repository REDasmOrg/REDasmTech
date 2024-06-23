#pragma once

#include "../listing.h"
#include "../segment.h"
#include <redasm/renderer.h>
#include <redasm/surface.h>
#include <redasm/theme.h>
#include <string>
#include <string_view>
#include <vector>

namespace redasm {

struct SurfaceRow {
    usize listingindex;
    std::vector<RDSurfaceCell> cells;
};

using SurfaceRows = std::vector<SurfaceRow>;

struct Renderer {
    explicit Renderer(usize f);
    [[nodiscard]] std::string word_at(usize row, usize col) const;
    [[nodiscard]] usize current_address() const;
    void swap(SurfaceRows& r) { m_rows.swap(r); }
    void clear() { m_rows.clear(); }
    void highlight_row(usize row);
    void highlight_words(usize row, usize col);
    void highlight_selection(RDSurfacePosition startsel,
                             RDSurfacePosition endsel);
    void highlight_cursor(usize row, usize col);
    void fill_columns();
    void set_current_item(LIndex lidx, const ListingItem& item);

    Renderer& new_row(const ListingItem& item);

    Renderer& character(SurfaceRow& row, char ch,
                        RDThemeKind fg = THEME_DEFAULT,
                        RDThemeKind bg = THEME_DEFAULT);

    Renderer& chunk(std::string_view arg, RDThemeKind fg = THEME_DEFAULT,
                    RDThemeKind bg = THEME_DEFAULT);

    [[nodiscard]] inline bool has_flag(usize f) const {
        return this->flags & f;
    }

    inline Renderer& arr_index(usize idx) {
        return this->chunk("[").constant(std::to_string(idx)).chunk("]");
    }

    inline Renderer& address(std::string_view arg) {
        return this->chunk(arg, THEME_ADDRESS);
    }

    inline Renderer& nop(std::string_view arg) {
        return this->chunk(arg, THEME_NOP);
    }

    inline Renderer& unknown() { return this->nop("?"); }

    inline Renderer& comment(std::string_view arg) {
        return this->chunk(arg, THEME_COMMENT);
    }

    inline Renderer& constant(std::string_view arg) {
        return this->chunk(arg, THEME_CONSTANT);
    }

    inline Renderer& string(std::string_view arg, bool quoted = true) {
        if(quoted)
            this->quote(arg, "\"", THEME_STRING);
        else
            this->chunk(arg, THEME_STRING);

        return *this;
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

    inline Renderer& quote(std::string_view arg, std::string_view q = "\"",
                           RDThemeKind fg = THEME_DEFAULT,
                           RDThemeKind bg = THEME_DEFAULT) {
        return this->chunk(q, fg, bg).chunk(arg, fg, bg).chunk(q, fg, bg);
    }

    inline Renderer& ws(usize n = 1) {
        return this->chunk(std::string(n, ' '));
    }

    static bool is_char_skippable(char ch);

public:
    usize columns{0};
    usize flags;

private:
    const Segment* current_segment() const;
    void check_current_segment(const ListingItem& item);

private:
    SurfaceRows m_rows;
    RDAddress m_curraddress{};
    LIndex m_listingidx{};
    usize m_currsegment{0};
};

} // namespace redasm
