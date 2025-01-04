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
    usize listingindex, length;
    std::vector<RDSurfaceCell> cells;
};

using SurfaceRows = std::vector<SurfaceRow>;

struct Renderer {
    explicit Renderer(usize f);
    [[nodiscard]] usize current_address() const;
    void swap(SurfaceRows& r) { m_rows.swap(r); }
    void highlight_row(int row);
    void highlight_words(int row, int col);
    void highlight_selection(RDSurfacePosition startsel,
                             RDSurfacePosition endsel);
    void highlight_cursor(int row, int col);
    void fill_columns();
    void set_current_item(LIndex lidx, const ListingItem& item);

    Renderer& new_row(const ListingItem& item);
    Renderer& constant(u64 c, int base = 0, int flags = 0,
                       RDThemeKind fg = THEME_CONSTANT);

    Renderer& reg(int reg);

    Renderer& character(SurfaceRow& row, char ch,
                        RDThemeKind fg = THEME_DEFAULT,
                        RDThemeKind bg = THEME_DEFAULT);

    Renderer& chunk(std::string_view arg, RDThemeKind fg = THEME_DEFAULT,
                    RDThemeKind bg = THEME_DEFAULT);

    [[nodiscard]] bool has_flag(usize f) const { return this->flags & f; }

    void clear() {
        m_rows.clear();
        this->autocolumns = 0;
    }

    Renderer& arr_index(usize idx) {
        return this->chunk("[").constant(idx).chunk("]");
    }

    Renderer& address(std::string_view arg) {
        return this->chunk(arg, THEME_ADDRESS);
    }

    Renderer& nop(std::string_view arg) { return this->chunk(arg, THEME_NOP); }

    Renderer& comment(std::string_view arg) {
        return this->chunk(arg, THEME_COMMENT);
    }

    Renderer& string(std::string_view arg, bool quoted = true) {
        if(quoted)
            this->quote(arg, "\"", THEME_STRING);
        else
            this->chunk(arg, THEME_STRING);

        return *this;
    }

    Renderer& type(std::string_view arg) {
        return this->chunk(arg, THEME_TYPE);
    }

    Renderer& unknown() {
        return this->chunk("???", THEME_BACKGROUND, THEME_FOREGROUND);
    }

    Renderer& function(std::string_view arg) {
        return this->chunk(arg, THEME_FUNCTION);
    }

    Renderer& word(std::string_view arg, RDThemeKind fg = THEME_DEFAULT,
                   RDThemeKind bg = THEME_DEFAULT) {
        return this->chunk(" ").chunk(arg, fg, bg).chunk(" ");
    }

    Renderer& quote(std::string_view arg, std::string_view q = "\"",
                    RDThemeKind fg = THEME_DEFAULT,
                    RDThemeKind bg = THEME_DEFAULT) {
        return this->chunk(q, fg, bg).chunk(arg, fg, bg).chunk(q, fg, bg);
    }

    Renderer& ws(usize n = 1) { return this->chunk(std::string(n, ' ')); }

    static std::string word_at(const SurfaceRows& rows, int row, int col);
    static bool is_char_skippable(char ch);

public: // High level interface
    Renderer& instr();
    Renderer& rdil();
    Renderer& addr(RDAddress address, int flags = 0);
    Renderer& int_i8(i8 v, int base, RDThemeKind fg = THEME_CONSTANT);
    Renderer& int_i16(i16 v, int base, RDThemeKind fg = THEME_CONSTANT);
    Renderer& int_i32(i32 v, int base, RDThemeKind fg = THEME_CONSTANT);
    Renderer& int_i64(i64 v, int base, RDThemeKind fg = THEME_CONSTANT);
    Renderer& int_u8(u8 v, int base, RDThemeKind fg = THEME_CONSTANT);
    Renderer& int_u16(u16 v, int base, RDThemeKind fg = THEME_CONSTANT);
    Renderer& int_u32(u32 v, int base, RDThemeKind fg = THEME_CONSTANT);
    Renderer& int_u64(u64 v, int base, RDThemeKind fg = THEME_CONSTANT);

public:
    bool prevmnemonic{false}; // Autoinsert whitespace on next chunk
    usize columns{0}, autocolumns{0};
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
