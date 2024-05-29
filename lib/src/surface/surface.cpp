#include "surface.h"
#include "../api/marshal.h"
#include "../builtins/processor.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

namespace {

template<typename T>
std::pair<bool, std::string> surface_checkpointer(const typing::ParsedType& pt,
                                                  T v) {
    Context* ctx = state::context;

    if(v) {
        if(pt.modifier == typing::TYPEMODIFIER_POINTER) {
            auto idx = ctx->address_to_index(static_cast<RDAddress>(v));
            if(idx)
                return {true, ctx->get_name(*idx)};
        }
        else if(pt.modifier == typing::TYPEMODIFIER_RELPOINTER) {
            RDAddress address = ctx->baseaddress + static_cast<RDAddress>(v);
            auto idx = ctx->address_to_index(address);
            if(idx)
                return {true, ctx->get_name(*idx)};
        }
    }

    return {false, ctx->to_hex(v, pt.type->size * 2)};
}

} // namespace

Surface::Surface() { m_renderer = std::make_unique<Renderer>(); }

usize Surface::current_index() const {
    usize idx =
        std::min(this->start + m_row, state::context->listing.size() - 1);
    return state::context->listing[idx].index;
}

const Segment* Surface::current_segment() const {
    usize idx = this->current_index();

    for(const Segment& s : state::context->segments) {
        if(idx >= s.index && idx < s.endindex)
            return &s;
    }

    return nullptr;
}

void Surface::render(usize s, usize n) {
    this->start = s;
    m_renderer->clear();

    const Listing& listing = state::context->listing;

    auto it = std::next(listing.begin(), this->start);
    if(it == listing.end())
        return;

    usize l = std::min<usize>(n, listing.size());

    for(usize i = 0; it != listing.end() && i < l; it++, i++) {
        m_renderer->set_current_item(*it);

        switch(it->type) {
            case ListingItemType::EMPTY: m_renderer->new_row(*it); break;
            case ListingItemType::HEX_DUMP: this->render_hexdump(*it); break;
            case ListingItemType::SEGMENT: this->render_segment(*it); break;
            case ListingItemType::FUNCTION: this->render_function(*it); break;
            case ListingItemType::BRANCH: this->render_branch(*it); break;
            case ListingItemType::CODE: this->render_code(*it); break;
            case ListingItemType::TYPE: this->render_type(*it); break;
            case ListingItemType::ARRAY: this->render_array(*it); break;
            default: break;
        }
    }

    m_renderer->fill_columns();
    m_renderer->highlight_row(m_row);

    if(!this->has_selection())
        m_renderer->highlight_words(m_row, m_col);

    auto startsel = this->start_selection(), endsel = this->end_selection();
    m_renderer->highlight_selection(startsel, endsel);

    m_renderer->highlight_cursor(m_row, m_col);
    m_renderer->swap(this->rows);
}

void Surface::seek(usize index) {
    this->start = index;

    if(this->start > state::context->listing.size())
        this->start = state::context->listing.size();

    this->clear_selection();
}

void Surface::set_columns(usize cols) { m_renderer->columns = cols; }

void Surface::set_position(usize row, usize col) {
    if(row == m_selrow && col == m_selcol)
        return;

    m_selrow = row;
    m_selcol = col;
    this->fit(m_selrow, m_selcol);
    this->select(m_selrow, m_selcol);
}

bool Surface::select_word(usize row, usize col) {
    if(row >= this->rows.size())
        return false;

    const SurfaceRow& cells = this->rows[row];

    if(col >= cells.size())
        col = cells.size() - 1;

    if(Renderer::is_char_skippable(cells[col].ch))
        return false;

    usize startcol = 0, endcol = 0;

    for(usize i = col; i-- > 0;) {
        RDSurfaceCell cell = cells[i];
        if(Renderer::is_char_skippable(cell.ch)) {
            startcol = i + 1;
            break;
        }
    }

    for(usize i = col; i < cells.size(); i++) {
        RDSurfaceCell cell = cells[i];
        if(Renderer::is_char_skippable(cell.ch)) {
            endcol = i - 1;
            break;
        }
    }

    if(startcol != endcol) {
        this->set_position(row, startcol);
        return this->select(row, endcol);
    }

    return false;
}

bool Surface::select(usize row, usize col) {
    if(row == m_row && col == m_col)
        return false;

    m_row = row;
    m_col = col;
    this->fit(m_row, m_col);
    return true;
}

RDSurfacePosition Surface::position() const { return {m_row, m_col}; }

RDSurfacePosition Surface::start_selection() const {
    if(m_row < m_selrow)
        return {m_row, m_col};

    if(m_row == m_selrow) {
        if(m_col < m_selcol)
            return {m_row, m_col};
    }

    return {m_selrow, m_selcol};
}

RDSurfacePosition Surface::end_selection() const {
    if(m_row > m_selrow)
        return {m_row, m_col};

    if(m_row == m_selrow) {
        if(m_col > m_selcol)
            return {m_row, m_col};
    }

    return {m_selrow, m_selcol};
}

std::string_view Surface::get_selected_text() const {
    auto [startrow, startcol] = this->start_selection();
    auto [endrow, endcol] = this->end_selection();

    m_strcache.clear();

    if(m_renderer->columns && m_strcache.capacity() < m_renderer->columns)
        m_strcache.reserve(m_renderer->columns * this->rows.size());

    for(auto i = startrow; i < this->rows.size() && i <= endrow; i++) {
        if(!m_strcache.empty())
            m_strcache += "\n";

        const SurfaceRow& row = this->rows[i];
        usize s = 0, e = 0;

        if(!row.empty())
            e = row.size() - 1;

        if(i == startrow)
            s = startcol;
        if(i == endrow)
            e = endcol;

        for(auto j = s; j <= e; j++)
            m_strcache.append(&row[j].ch, 1);
    }

    return m_strcache;
}

std::string_view Surface::get_text() const {
    m_strcache.clear();

    if(m_renderer->columns && m_strcache.capacity() < m_renderer->columns)
        m_strcache.reserve(m_renderer->columns * this->rows.size());

    for(const SurfaceRow& row : this->rows) {
        if(!m_strcache.empty())
            m_strcache += "\n";

        for(RDSurfaceCell col : row)
            m_strcache.append(&col.ch, 1);
    }

    return m_strcache;
}

bool Surface::has_selection() const {
    return (m_row != m_selrow) || (m_col != m_selcol);
}

void Surface::render_hexdump(const ListingItem& item) {
    static constexpr usize HEX_WIDTH = 16;

    const Memory* memory = state::context->memory.get();
    usize c = 0;
    m_renderer->new_row(item);

    for(usize i = item.start_index; i < item.end_index; i++, c += 3) {
        Byte b = memory->at(i);

        if(b.has_byte())
            m_renderer->chunk(state::context->to_hex(b.byte(), 2));
        else
            m_renderer->nop("??");

        m_renderer->chunk(" ");
    }

    if(c < HEX_WIDTH * 3)
        m_renderer->chunk(std::string((HEX_WIDTH * 3) - c, ' '));

    c = 0;

    for(usize i = item.start_index; i < item.end_index; i++, c++) {
        Byte b = memory->at(i);

        if(b.has_byte()) {
            std::string s{std::isprint(b.byte()) ? static_cast<char>(b.byte())
                                                 : '.'};
            m_renderer->chunk(s);
        }
        else
            m_renderer->nop("?");
    }

    if(c < HEX_WIDTH)
        m_renderer->chunk(std::string(HEX_WIDTH - c, ' '));
}

void Surface::render_branch(const ListingItem& item) {
    std::string name = state::context->get_name(item.index);
    m_renderer->new_row(item).chunk(name + ":", THEME_ADDRESS);
}

void Surface::render_segment(const ListingItem& item) {
    m_renderer->new_row(item);

    const Database& db = state::context->database;
    const AddressDetail& d = db.get_detail(item.index);

    RDRendererParams rp{};
    rp.renderer = api::to_c(m_renderer.get());
    rp.address = m_renderer->current_address();
    rp.segment_index = d.segment_index;

    const RDProcessor* p = state::context->processor;
    assume(p);

    if(!p->rendersegment || !p->rendersegment(p, &rp))
        assume(builtins::processor::render_segment(&rp));
}

void Surface::render_function(const ListingItem& item) {
    m_renderer->new_row(item);

    const RDProcessor* p = state::context->processor;
    assume(p);

    RDRendererParams rp{};
    rp.renderer = api::to_c(m_renderer.get());
    rp.address = m_renderer->current_address();

    if(!p->renderfunction || !p->renderfunction(p, &rp))
        assume(builtins::processor::render_function(&rp));
}

void Surface::render_type(const ListingItem& item) {
    assume(item.parsed_type_context);
    assume(item.parsed_type);

    tl::optional<typing::ParsedType> pt = item.parsed_type_context;
    const Memory* memory = state::context->memory.get();
    std::string fname;

    if(item.field_index) {
        assume(*item.field_index < pt->type->dict.size());

        auto f = pt->type->dict.at(*item.field_index);
        fname = f.second;
        pt = item.parsed_type;
    }
    else
        fname = state::context->get_name(item.index);

    assume(pt);
    std::string t = state::context->types.to_string(*pt);

    m_renderer->new_row(item);

    if(item.array_index)
        m_renderer->arr_index(*item.array_index);
    else if(pt->type->id() != typing::types::STRUCT)
        m_renderer->type(t).ws().chunk(fname);

    switch(pt->type->id()) {
        case typing::types::CHAR:
        case typing::types::WCHAR: {
            tl::optional<char> ch;

            if(pt->type->is_wide())
                ch = memory->get_wchar(item.index);
            else
                ch = memory->get_char(item.index);

            assume(ch.has_value());
            m_renderer->word("=").ws().quote(std::string_view{&ch.value(), 1},
                                             "\'");
            break;
        }

        case typing::types::I8:
        case typing::types::U8: {
            auto v = memory->get_u8(item.index);
            assume(v.has_value());

            auto [isptr, val] = surface_checkpointer(*pt, *v);
            m_renderer->word("=").chunk(val,
                                        isptr ? THEME_ADDRESS : THEME_CONSTANT);
            break;
        }

        case typing::types::I16:
        case typing::types::U16: {
            auto v = memory->get_u16(item.index, pt->type->is_big());
            assume(v.has_value());

            auto [isptr, val] = surface_checkpointer(*pt, *v);
            m_renderer->word("=").chunk(val,
                                        isptr ? THEME_ADDRESS : THEME_CONSTANT);
            break;
        }

        case typing::types::I32:
        case typing::types::U32: {
            auto v = memory->get_u32(item.index, pt->type->is_big());
            assume(v.has_value());

            auto [isptr, val] = surface_checkpointer(*pt, *v);
            m_renderer->word("=").chunk(val,
                                        isptr ? THEME_ADDRESS : THEME_CONSTANT);
            break;
        }

        case typing::types::I64:
        case typing::types::U64: {
            auto v = memory->get_u64(item.index, pt->type->is_big());
            assume(v.has_value());
            m_renderer->word("=").constant(state::context->to_hex(*v, 16));
            break;
        }

        case typing::types::STR:
        case typing::types::WSTR: {
            tl::optional<std::string> v;

            if(pt->type->is_wide())
                v = state::context->memory->get_wstring(item.index);
            else
                v = state::context->memory->get_string(item.index);

            assume(v.has_value());
            m_renderer->ws()
                .chunk("\"")
                .string(*v)
                .chunk("\"")
                .chunk(",")
                .constant("0");
            break;
        }

        case typing::types::STRUCT: {
            if(!item.array_index) {
                std::string label = state::context->get_name(item.index);
                m_renderer->function("struct")
                    .ws()
                    .type(pt->type->name)
                    .ws()
                    .chunk(label);
            }

            m_renderer->word("=");
            break;
        }

        default: unreachable; break;
    }
}

void Surface::render_code(const ListingItem& item) {
    m_renderer->new_row(item);

    RDRendererParams rp{};
    rp.renderer = api::to_c(m_renderer.get());
    rp.address = m_renderer->current_address();

    const RDProcessor* p = state::context->processor;
    assume(p);

    if(!p->renderinstruction || !p->renderinstruction(p, &rp))
        m_renderer->chunk("???");
}

void Surface::render_array(const ListingItem& item) {
    assume(item.parsed_type);

    auto pt = *item.parsed_type;
    std::string chars;

    if(pt.type->is_char()) {
        const Memory* memory = state::context->memory.get();
        usize idx = item.index;
        chars += "\"";

        for(usize i = 0; i < pt.n && idx < memory->size();
            i++, idx += pt.type->size) {
            auto b = memory->get_type(idx, pt.type->name);

            if(!b) {
                chars += "\",?\"";
                continue;
            }

            if(std::isprint(b->ch_v))
                chars += b->ch_v;
            else
                chars += fmt::format("\\x{}", static_cast<int>(b->ch_v));
        }

        chars += "\"";
    }

    if(item.field_index) {
        auto ptc = *item.parsed_type_context;
        assume(*item.field_index < ptc.type->dict.size());
        auto field = ptc.type->dict[*item.field_index];

        m_renderer->new_row(item);

        if(pt.type->id() == typing::types::STRUCT)
            m_renderer->function("struct ");

        m_renderer->type(state::context->types.to_string(pt))
            .ws()
            .chunk(field.second)
            .word("=")
            .string(chars);
    }
    else {
        m_renderer->new_row(item);

        if(pt.type->id() == typing::types::STRUCT)
            m_renderer->function("struct ");

        std::string name = state::context->get_name(item.index);
        m_renderer->type(state::context->types.to_string(pt)).ws().chunk(name);

        if(item.array_index)
            m_renderer->arr_index(*item.array_index);

        m_renderer->word("=").string(chars);
    }
}

void Surface::fit(usize& row, usize& col) {
    if(this->rows.empty()) {
        row = 0;
        col = 0;
    }
    else {
        if(row >= this->rows.size())
            row = this->rows.size() - 1;

        if(this->rows[row].empty())
            col = 0;
        else if(col >= this->rows[row].size())
            col = this->rows[row].size() - 1;
    }
}

void Surface::clear_selection() {
    m_selrow = m_row;
    m_selcol = m_col;
}

} // namespace redasm
