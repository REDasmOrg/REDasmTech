#include "renderer.h"
#include "../api/marshal.h"
#include "../context.h"
#include "../memory/memory.h"
#include "../rdil/rdil.h"
#include "../state.h"
#include "../utils/utils.h"
#include <cctype>

namespace redasm {

namespace {

template<typename T>
std::string_view render_int(T val, int base) {
    constexpr int W = sizeof(T) * 2;
    constexpr bool SIGN = std::is_signed_v<T>;
    return utils::to_string(val, base, SIGN, W);
}

} // namespace

Renderer::Renderer(usize f): flags{f} {}
usize Renderer::current_address() const { return m_curraddress; }

const RDSegment* Renderer::current_segment() const {
    if(m_segmidx >= state::context->program.segments.length) return nullptr;
    return &state::context->program.segments.data[m_segmidx];
}

void Renderer::highlight_row(int row) {
    if(this->has_flag(SURFACE_NOCURSORLINE)) return;

    if(row >= static_cast<int>(m_rows.size())) return;

    SurfaceRow& sfrow = m_rows[row];

    for(RDSurfaceCell& cell : sfrow.cells) {
        if(cell.bg == THEME_DEFAULT) cell.bg = THEME_SEEK;
    }
}

void Renderer::highlight_words(int row, int col) {
    if(this->has_flag(SURFACE_NOHIGHLIGHT)) return;

    std::string word = Renderer::word_at(m_rows, row, col);
    if(word.empty()) return;

    for(SurfaceRow& row : m_rows) {
        for(usize i = 0; i < row.cells.size(); i++) {
            usize endidx = i;
            bool found = true;

            for(char ch : word) {
                if(endidx >= row.cells.size()) break;

                RDSurfaceCell cell = row.cells[endidx++];

                if(cell.ch == ch) continue;

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
    if(this->has_flag(SURFACE_NOSELECTION)) return;

    for(int row = startsel.row;
        row < static_cast<int>(m_rows.size()) && row <= endsel.row; row++) {
        int startcol = 0, endcol = 0;

        if(!m_rows[row].cells.empty()) endcol = m_rows[row].cells.size() - 1;

        if(row == startsel.row) startcol = startsel.col;
        if(row == endsel.row) endcol = endsel.col;

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
    if(this->has_flag(SURFACE_NOCURSOR) || m_rows.empty()) return;

    if(row >= static_cast<int>(m_rows.size())) row = m_rows.size() - 1;

    if(m_rows[row].cells.empty()) return;

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

        ct_assume(row.length <= row.cells.size());
    }
}

void Renderer::set_current_item(LIndex lidx, const ListingItem& item) {
    m_curraddress = item.address;
    m_listingidx = lidx;
    this->check_current_segment(item);
}

Renderer& Renderer::instr() {
    const Context* ctx = state::context;
    const RDProcessorPlugin* p = ctx->processorplugin;
    ct_assume(p);

    RDInstruction instr{};

    if(!ctx->worker->emulator.decode(this->current_address(), instr))
        this->unknown();
    else
        p->render_instruction(ctx->processor, api::to_c(this), &instr);

    return *this;
}

Renderer& Renderer::rdil() {
    const Context* ctx = state::context;
    const RDProcessorPlugin* p = ctx->processorplugin;
    ct_assume(p);

    rdil::ILExprList el;
    RDInstruction instr{};
    bool ok = ctx->worker->emulator.decode(this->current_address(), instr);

    if(!ok || !p->lift || !p->lift(ctx->processor, api::to_c(&el), &instr))
        el.clear();

    if(el.empty()) el.append(el.expr_unknown());

    for(usize i = 0; i < el.size(); i++) {
        if(i) this->chunk("; ");

        rdil::render(el.at(i), *this);
    }

    return *this;
}

Renderer& Renderer::addr(RDAddress address, int flags) {
    const Context* ctx = state::context;
    const RDSegment* seg = ctx->program.find_segment(address);

    if(seg && memory::has_flag(seg, address, BF_REFSTO)) {
        if(flags == RC_NEEDSIGN) {
            if(static_cast<i64>(address) < 0)
                this->chunk("-");
            else
                this->chunk("+");
        }

        this->chunk(ctx->get_name(address), THEME_ADDRESS);
    }
    else
        this->constant(static_cast<u64>(address), 16, flags, THEME_ADDRESS);

    return *this;
}

Renderer& Renderer::int_i8(i8 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
}

Renderer& Renderer::int_i16(i16 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
}

Renderer& Renderer::int_i32(i32 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
}

Renderer& Renderer::int_i64(i64 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
}

Renderer& Renderer::int_u8(u8 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
}

Renderer& Renderer::int_u16(u16 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
}

Renderer& Renderer::int_u32(u32 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
}

Renderer& Renderer::int_u64(u64 v, int base, RDThemeKind fg) {
    return this->chunk(render_int(v, base), fg);
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

    if(this->columns) m_rows.back().cells.reserve(this->columns);

    if(!this->has_flag(SURFACE_NOADDRESS)) {
        const RDSegment* s = this->current_segment();
        if(s) this->chunk(s->name).chunk(":");

        const RDProcessorPlugin* p = state::context->processorplugin;
        ct_assume(p);
        this->chunk(utils::to_hex(m_curraddress, s ? s->bits : -1)).ws(2);

        if(item.indent) this->ws(item.indent);
    }

    return *this;
}

Renderer& Renderer::constant(u64 c, int base, int flags, RDThemeKind fg) {
    if(c == 0) { // Simplified logic
        this->chunk("0", fg);
        return *this;
    }

    if(!base) base = 16;

    auto sc = static_cast<i64>(c);

    if(sc < 0) {
        if(flags == RC_NOSIGN)
            this->chunk(utils::to_string(std::abs(sc), base), fg);
        else
            this->chunk(utils::to_string(sc, base, true), fg);
    }
    else {
        if(flags == RC_NEEDSIGN) this->chunk("+", fg);
        this->chunk(utils::to_string(c, base), fg);
    }

    return *this;
}

Renderer& Renderer::mnem(const RDInstruction* instr, RDThemeKind fg) {
    const Context* ctx = state::context;
    const RDProcessorPlugin* p = ctx->processorplugin;
    ct_assume(p);

    std::string_view mnemstr;

    if(p->get_mnemonic) {
        const char* res = p->get_mnemonic(ctx->processor, instr);
        if(res) mnemstr = res;
    }

    if(mnemstr.empty()) mnemstr = utils::to_string(instr->id, 10);
    this->chunk(mnemstr, fg);
    this->prevmnemonic = true;
    return *this;
}

Renderer& Renderer::reg(int reg) {
    const Context* ctx = state::context;
    const RDProcessorPlugin* p = ctx->processorplugin;
    ct_assume(p);

    std::string_view regname;

    if(p->get_registername) {
        const char* res = p->get_registername(ctx->processor, reg);
        if(res) regname = res;
    }

    if(regname.empty()) regname = utils::to_string(reg, 10);
    return this->chunk(regname, THEME_REG);
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
        if(this->columns && row.cells.size() >= this->columns) break;

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
    if(m_segmidx < state::context->program.segments.length) {
        const RDSegment& s = state::context->program.segments.data[m_segmidx];
        if(item.address >= s.start && item.address < s.end) return;
    }

    for(isize i = 0; i < state::context->program.segments.length; i++) {
        const RDSegment& s = state::context->program.segments.data[i];

        if(item.address >= s.start && item.address < s.end) {
            m_segmidx = i;
            return;
        }
    }

    m_segmidx = state::context->program.segments.length;
}

std::string Renderer::word_at(const SurfaceRows& rows, int row, int col) {
    if(row >= static_cast<int>(rows.size())) return {};

    const SurfaceRow& sfrow = rows[row];

    if(col >= static_cast<int>(sfrow.cells.size()))
        col = sfrow.cells.size() - 1;

    if(Renderer::is_char_skippable(sfrow.cells[col].ch)) return {};

    std::string word;

    for(int i = col; i-- > 0;) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(Renderer::is_char_skippable(cell.ch)) break;
        word.insert(0, &cell.ch, 1);
    }

    for(int i = col; i < static_cast<int>(sfrow.cells.size()); i++) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(Renderer::is_char_skippable(cell.ch)) break;
        word.append(&cell.ch, 1);
    }

    return word;
}

bool Renderer::is_char_skippable(char ch) {
    if(ch == '_' || ch == '@' || ch == '.') return false;
    return std::isspace(ch) || std::ispunct(ch);
}

} // namespace redasm
