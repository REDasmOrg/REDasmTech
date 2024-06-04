#include "renderer.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"
#include <cctype>

namespace redasm {

Renderer::Renderer(usize f): flags{f} {}
usize Renderer::current_address() const { return m_curraddress; }

const Segment* Renderer::current_segment() const {
    if(m_currsegment >= state::context->segments.size())
        return nullptr;

    return &state::context->segments[m_currsegment];
}

void Renderer::highlight_row(usize idx) {
    if(this->has_flag(SURFACE_NOCURSORLINE))
        return;

    if(idx >= m_rows.size())
        return;

    SurfaceRow& row = m_rows[idx];

    for(RDSurfaceCell& cell : row.cells)
        cell.bg = THEME_SEEK;
}

void Renderer::highlight_words(usize row, usize col) {
    if(this->has_flag(SURFACE_NOHIGHLIGHT))
        return;

    std::string word = this->word_at(row, col);
    if(word.empty())
        return;

    for(SurfaceRow& row : m_rows) {
        for(usize i = 0; i < row.cells.size(); i++) {
            usize endidx = i;
            bool found = true;

            for(char ch : word) {
                if(endidx >= row.cells.size())
                    break;

                RDSurfaceCell cell = row.cells[endidx++];

                if(cell.ch == ch)
                    continue;

                found = false;
                endidx = i;
                break;
            }

            for(usize j = i; found && j < endidx; j++) {
                RDSurfaceCell& cell = row.cells[j];
                cell.fg = THEME_HIGHLIGHTFG;
                cell.bg = THEME_HIGHLIGHTBG;
            }
        }
    }
}

void Renderer::highlight_selection(RDSurfacePosition startsel,
                                   RDSurfacePosition endsel) {
    if(this->has_flag(SURFACE_NOSELECTION))
        return;

    for(usize row = startsel.row; row < m_rows.size() && row <= endsel.row;
        row++) {
        usize startcol = 0, endcol = 0;

        if(!m_rows[row].cells.empty())
            endcol = m_rows[row].cells.size() - 1;

        if(row == startsel.row)
            startcol = startsel.col;
        if(row == endsel.row)
            endcol = endsel.col;

        for(usize col = startcol;
            col < m_rows[row].cells.size() && col <= endcol; col++) {
            RDSurfaceCell& c = m_rows[row].cells[col];
            c.fg = THEME_SELECTIONFG;
            c.bg = THEME_SELECTIONBG;
        }
    }
}

void Renderer::highlight_cursor(usize row, usize col) {
    if(this->has_flag(SURFACE_NOCURSOR) || m_rows.empty())
        return;

    if(row >= m_rows.size())
        row = m_rows.size() - 1;

    if(m_rows[row].cells.empty())
        return;

    if(col >= m_rows[row].cells.size())
        col = m_rows[row].cells.size() - 1;

    m_rows[row].cells[col].fg = THEME_CURSORFG;
    m_rows[row].cells[col].bg = THEME_CURSORBG;
}

void Renderer::fill_columns() {
    for(SurfaceRow& row : m_rows) {
        while(row.cells.size() < this->columns)
            this->character(row, ' ');
    }
}

void Renderer::set_current_item(usize lidx, const ListingItem& item) {
    auto addr = state::context->index_to_address(item.index);
    assume(addr);
    m_curraddress = *addr;
    m_listingidx = lidx;
    this->check_current_segment(item);
}

Renderer& Renderer::new_row(const ListingItem& item) {
    m_rows.emplace_back(SurfaceRow{
        m_listingidx,
        {},
    });

    if(this->columns)
        m_rows.back().cells.reserve(this->columns);

    if(!this->has_flag(SURFACE_NOADDRESS)) {
        if(const Segment* s = this->current_segment(); s)
            this->chunk(s->name).chunk(":");

        this->chunk(state::context->to_hex(m_curraddress)).ws(2);

        if(item.indent)
            this->ws(item.indent);
    }

    return *this;
}

Renderer& Renderer::character(SurfaceRow& row, char ch, RDThemeKind fg,
                              RDThemeKind bg) {
    row.cells.emplace_back(RDSurfaceCell{ch, fg, bg});
    return *this;
}

Renderer& Renderer::chunk(std::string_view arg, RDThemeKind fg,
                          RDThemeKind bg) {
    SurfaceRow& row = m_rows.back();

    for(char ch : arg) {
        if(this->columns && row.cells.size() >= this->columns)
            break;

        switch(ch) {
            case '\t':
                this->character(row, '\\', fg, bg);
                this->character(row, 't', fg, bg);
                break;

            case '\n':
                this->character(row, '\\', fg, bg);
                this->character(row, 'n', fg, bg);
                break;

            case '\r':
                this->character(row, '\\', fg, bg);
                this->character(row, 'r', fg, bg);
                break;

            case '\v':
                this->character(row, '\\', fg, bg);
                this->character(row, 'v', fg, bg);
                break;

            default: this->character(row, ch, fg, bg); break;
        }
    }

    return *this;
}

void Renderer::check_current_segment(const ListingItem& item) {
    if(m_currsegment < state::context->segments.size()) {
        const Segment& s = state::context->segments[m_currsegment];

        if(item.index >= s.index && item.index < s.endindex)
            return;
    }

    for(usize i = 0; i < state::context->segments.size(); i++) {
        const Segment& s = state::context->segments[i];

        if(item.index >= s.index && item.index < s.endindex) {
            m_currsegment = i;
            return;
        }
    }

    m_currsegment = state::context->segments.size();
}

std::string Renderer::word_at(usize row, usize col) const {
    if(row > m_rows.size())
        return {};

    const SurfaceRow& sfrow = m_rows[row];

    if(col >= sfrow.cells.size())
        col = sfrow.cells.size() - 1;

    if(is_char_skippable(sfrow.cells[col].ch))
        return {};

    std::string word;

    for(usize i = col; i-- > 0;) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(is_char_skippable(cell.ch))
            break;

        word.insert(0, &cell.ch, 1);
    }

    for(usize i = col; i < sfrow.cells.size(); i++) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(is_char_skippable(cell.ch))
            break;

        word.append(&cell.ch, 1);
    }

    return word;
}

bool Renderer::is_char_skippable(char ch) {
    if(ch == '_' || ch == '.')
        return false;

    return std::isspace(ch) || std::ispunct(ch);
}

} // namespace redasm
