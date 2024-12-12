#include "renderer.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../error.h"
#include "../rdil/rdil.h"
#include "../state.h"
#include "../utils/utils.h"
#include <cctype>

namespace redasm {

Renderer::Renderer(usize f): flags{f} {}
usize Renderer::current_address() const { return m_curraddress; }

const Segment* Renderer::current_segment() const {
    if(m_currsegment >= state::context->segments.size())
        return nullptr;

    return &state::context->segments[m_currsegment];
}

void Renderer::highlight_row(int row) {
    if(this->has_flag(SURFACE_NOCURSORLINE))
        return;

    if(row >= static_cast<int>(m_rows.size()))
        return;

    SurfaceRow& sfrow = m_rows[row];

    for(RDSurfaceCell& cell : sfrow.cells)
        cell.bg = THEME_SEEK;
}

void Renderer::highlight_words(int row, int col) {
    if(this->has_flag(SURFACE_NOHIGHLIGHT))
        return;

    std::string word = Renderer::word_at(m_rows, row, col);
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

    for(int row = startsel.row;
        row < static_cast<int>(m_rows.size()) && row <= endsel.row; row++) {
        int startcol = 0, endcol = 0;

        if(!m_rows[row].cells.empty())
            endcol = m_rows[row].cells.size() - 1;

        if(row == startsel.row)
            startcol = startsel.col;
        if(row == endsel.row)
            endcol = endsel.col;

        for(int col = startcol;
            col < static_cast<int>(m_rows[row].cells.size()) && col <= endcol;
            col++) {
            RDSurfaceCell& c = m_rows[row].cells[col];
            c.fg = THEME_SELECTIONFG;
            c.bg = THEME_SELECTIONBG;
        }
    }
}

void Renderer::highlight_cursor(int row, int col) {
    if(this->has_flag(SURFACE_NOCURSOR) || m_rows.empty())
        return;

    if(row >= static_cast<int>(m_rows.size()))
        row = m_rows.size() - 1;

    if(m_rows[row].cells.empty())
        return;

    if(col >= static_cast<int>(m_rows[row].cells.size()))
        col = m_rows[row].cells.size() - 1;

    m_rows[row].cells[col].fg = THEME_CURSORFG;
    m_rows[row].cells[col].bg = THEME_CURSORBG;
}

void Renderer::fill_columns() {
    usize ncols = this->columns > 0 ? this->columns : this->autocolumns;

    for(SurfaceRow& row : m_rows) {
        row.length = row.cells.size();

        while(row.cells.size() < ncols)
            this->character(row, ' ');

        assume(row.length <= row.cells.size());
    }
}

void Renderer::set_current_item(LIndex lidx, const ListingItem& item) {
    auto addr = state::context->index_to_address(item.index);
    assume(addr);
    m_curraddress = *addr;
    m_listingidx = lidx;
    this->check_current_segment(item);
}

Renderer& Renderer::instr() {
    const RDProcessor* p = state::context->processor;
    assume(p);

    RDInstruction instr = {
        .address = this->current_address(),
    };

    p->decode(p, &instr);

    if(!instr.length || !p->renderinstruction)
        this->chunk("???");
    else
        p->renderinstruction(p, api::to_c(this), &instr);

    return *this;
}

Renderer& Renderer::rdil() {
    const RDProcessor* p = state::context->processor;
    assume(p);

    rdil::ILExprList el;

    RDInstruction instr{
        .address = this->current_address(),
    };

    if(p->decode)
        p->decode(p, &instr);

    if(!instr.length || !p->lift || !p->lift(p, api::to_c(&el), &instr))
        el.clear();

    if(el.empty())
        el.append(el.expr_unknown());

    for(usize i = 0; i < el.size(); i++) {
        if(i)
            this->chunk("; ");

        rdil::render(el.at(i), *this);
    }

    return *this;
}

Renderer& Renderer::addr(RDAddress address, int flags) {
    const Context* ctx = state::context;

    ctx->address_to_index(address)
        .and_then([&](MIndex idx) -> tl::optional<MIndex> {
            if(ctx->memory->at(idx).has(BF_REFSTO)) {
                if(flags == RC_NEEDSIGN) {
                    if(static_cast<i64>(address) < 0)
                        this->chunk("-");
                    else
                        this->chunk("+");
                }

                this->chunk(ctx->get_name(idx), THEME_ADDRESS);
                return idx;
            }
            return tl::nullopt;
        })
        .or_else([&]() {
            this->constant(static_cast<u64>(address), 16, flags, THEME_ADDRESS);
        });

    return *this;
}

Renderer& Renderer::new_row(const ListingItem& item) {
    this->prevmnemonic = false;

    if(!this->columns && !m_rows.empty()) {
        this->autocolumns =
            std::max(this->autocolumns, m_rows.back().cells.size());
    }

    m_rows.emplace_back(SurfaceRow{
        .listingindex = m_listingidx,
        .cells = {},
    });

    if(this->columns)
        m_rows.back().cells.reserve(this->columns);

    if(!this->has_flag(SURFACE_NOADDRESS)) {
        if(const Segment* s = this->current_segment(); s)
            this->chunk(s->name).chunk(":");

        this->chunk(state::context->to_hex(m_curraddress, 0)).ws(2);

        if(item.indent)
            this->ws(item.indent);
    }

    return *this;
}

Renderer& Renderer::constant(u64 c, int base, int flags, RDThemeKind fg) {
    if(c == 0) { // Simplified logic
        this->chunk("0", fg);
        return *this;
    }

    if(!base)
        base = 16;

    auto sc = static_cast<i64>(c);

    if(sc < 0) {
        if(flags == RC_NOSIGN)
            this->chunk(utils::to_string(std::abs(sc), base), fg);
        else
            this->chunk(utils::to_string(sc, base, true), fg);
    }
    else {
        if(flags == RC_NEEDSIGN)
            this->chunk("+", fg);
        this->chunk(utils::to_string(c, base), fg);
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
    if(this->prevmnemonic) {
        this->prevmnemonic = false;
        this->ws();
    }

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

std::string Renderer::word_at(const SurfaceRows& rows, int row, int col) {
    if(row >= static_cast<int>(rows.size()))
        return {};

    const SurfaceRow& sfrow = rows[row];

    if(col >= static_cast<int>(sfrow.cells.size()))
        col = sfrow.cells.size() - 1;

    if(Renderer::is_char_skippable(sfrow.cells[col].ch))
        return {};

    std::string word;

    for(int i = col; i-- > 0;) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(Renderer::is_char_skippable(cell.ch))
            break;

        word.insert(0, &cell.ch, 1);
    }

    for(int i = col; i < static_cast<int>(sfrow.cells.size()); i++) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(Renderer::is_char_skippable(cell.ch))
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
