#include "surface.h"
#include "../api/marshal.h"
#include "../builtins/processor.h"
#include "../context.h"
#include "../state.h"
#include "../typing/base.h"
#include "../utils/utils.h"
#include <algorithm>
#include <redasm/listing.h>

namespace redasm {

namespace {

constexpr usize COLUMN_PADDING = 6;

template<typename T>
tl::optional<std::string> surface_checkaddr(T v, bool& isaddr) {
    const Context* ctx = state::context;
    isaddr = ctx->is_address(v);

    if(isaddr) {
        return ctx->address_to_index(static_cast<RDAddress>(v))
            .map([&](MIndex idx) { return ctx->get_name(idx); });
    }

    return tl::nullopt;
}

std::string surface_valuestr(const typing::Value& v, bool& isaddr) {
    assume(!v.type.n);

    const Context* ctx = state::context;

    switch(v.type.id) {
        case typing::ids::I8: {
            if(v.i8_v < 0)
                return std::to_string(v.i8_v);
            return surface_checkaddr(v.i8_v, isaddr)
                .value_or(ctx->to_hex(v.i8_v, sizeof(i8) * 2));
        }

        case typing::ids::I16:
        case typing::ids::I16BE: {
            if(v.i16_v < 0)
                return std::to_string(v.i16_v);
            return surface_checkaddr(v.i16_v, isaddr)
                .value_or(ctx->to_hex(v.i16_v, sizeof(i16) * 2));
        }

        case typing::ids::I32:
        case typing::ids::I32BE: {
            if(v.i32_v < 0)
                return std::to_string(v.i32_v);
            return surface_checkaddr(v.i32_v, isaddr)
                .value_or(ctx->to_hex(v.i32_v, sizeof(i32) * 2));
        }

        case typing::ids::I64:
        case typing::ids::I64BE: {
            if(v.i64_v < 0)
                return std::to_string(v.i64_v);
            return surface_checkaddr(v.i64_v, isaddr)
                .value_or(ctx->to_hex(v.i64_v, sizeof(i64) * 2));
        }

        case typing::ids::U8: {
            return surface_checkaddr(v.u8_v, isaddr)
                .value_or(ctx->to_hex(v.u8_v, sizeof(u8) * 2));
        }

        case typing::ids::U16:
        case typing::ids::U16BE: {
            return surface_checkaddr(v.u16_v, isaddr)
                .value_or(ctx->to_hex(v.u16_v, sizeof(u16) * 2));
        }

        case typing::ids::U32:
        case typing::ids::U32BE: {
            return surface_checkaddr(v.u32_v, isaddr)
                .value_or(ctx->to_hex(v.u32_v, sizeof(u32) * 2));
        }

        case typing::ids::U64:
        case typing::ids::U64BE: {
            return surface_checkaddr(v.u64_v, isaddr)
                .value_or(ctx->to_hex(v.u64_v, sizeof(u64) * 2));
        }

        default: break;
    }

    unreachable;
}

} // namespace

Surface::Surface(usize flags) {
    m_renderer = std::make_unique<Renderer>(flags);

    this->lock_history([&]() { this->jump_to_ep(); });
}

tl::optional<LIndex> Surface::current_listing_index() const {
    if(!this->start || this->start >= state::context->listing.size() ||
       state::context->listing.empty())
        return tl::nullopt;

    return std::min(*this->start + m_row, state::context->listing.size() - 1);
}

tl::optional<MIndex> Surface::current_index() const {
    return this->current_listing_index().map(
        [](LIndex index) { return state::context->listing[index].index; });
}

tl::optional<usize> Surface::index_under_pos(RDSurfacePosition pos) const {
    if(!this->start)
        return tl::nullopt;

    if(std::string w = Renderer::word_at(this->rows, pos.row, pos.col);
       !w.empty()) {
        return state::context->get_index(w);
    }

    return tl::nullopt;
}

tl::optional<usize> Surface::index_under_cursor() const {
    return this->index_under_pos({
        .row = m_row,
        .col = m_col,
    });
}

const Segment* Surface::current_segment() const {
    if(auto idx = this->current_index(); idx) {
        for(const Segment& s : state::context->segments) {
            if(*idx >= s.index && *idx < s.endindex)
                return &s;
        }
    }

    return nullptr;
}

const Function* Surface::current_function() const {
    if(auto idx = this->current_index(); idx)
        return state::context->index_to_function(*idx);

    return nullptr;
}

void Surface::render_function(const Function& f) {
    m_renderer->clear();

    const Listing& listing = state::context->listing;

    for(const Function::BasicBlock& bb : f.blocks) {
        auto startit = listing.lower_bound(bb.start);
        assume(startit != listing.end());
        auto endit = listing.upper_bound(bb.end, startit);
        assume(endit != listing.end());

        LIndex startidx = std::distance(listing.begin(), startit);
        LIndex n = std::distance(startit, endit);
        this->render_range(startidx, n);
    }

    this->render_finalize();
}

void Surface::render(usize n) {
    m_renderer->clear();
    this->start.map([&](LIndex s) { this->render_range(s, n); });
    this->render_finalize();
}

bool Surface::go_back() {
    if(m_histback.empty())
        return false;

    HistoryItem hitem = m_histback.front();
    m_histback.pop_front();

    this->update_history(m_histforward);

    this->lock_history([&]() {
        this->start = hitem.start;
        this->set_position(hitem.row, hitem.col);
    });

    return true;
}

bool Surface::go_forward() {
    if(m_histforward.empty())
        return false;

    HistoryItem hitem = m_histforward.front();
    m_histforward.pop_front();

    this->update_history(m_histback);

    this->lock_history([&]() {
        this->start = hitem.start;
        this->set_position(hitem.row, hitem.col);
    });

    return true;
}

void Surface::seek_position(LIndex index) {
    if(!this->rows.empty()) {
        LIndex s = this->rows.front().listingindex;
        LIndex e = this->rows.back().listingindex;

        if(index >= s && index <= e) {
            for(usize i = 0; i < this->rows.size(); i++) {
                if(this->rows[i].listingindex == index) {
                    this->set_position(i, 0);
                    return;
                }
            }
        }
    }

    this->seek(index);
}

void Surface::seek(LIndex index) { // Seek doesn't update back/forward stack
    this->start = std::min(index, state::context->listing.size());
}

bool Surface::jump_to(MIndex index) {
    Context* ctx = state::context;
    auto it = ctx->listing.lower_bound(index);

    if(it != ctx->listing.end()) {
        this->update_history(m_histback);
        this->start = std::distance(ctx->listing.cbegin(), it);
        return true;
    }

    return false;
}

bool Surface::jump_to_ep() {
    bool res = false;
    this->lock_history([&]() {
        Context* ctx = state::context;

        if(!ctx->entrypoints.empty())
            res = this->jump_to(ctx->entrypoints.front());
    });
    return res;
}

const std::vector<RDSurfacePath>& Surface::get_path() const {
    const Context* ctx = state::context;
    const Listing& lst = ctx->listing;
    const auto& mem = ctx->memory;

    m_path.clear();
    m_done.clear();

    for(usize i = 0; this->start && i < this->rows.size(); i++) {
        usize lidx = *this->start + i;
        if(lidx >= lst.size())
            break;

        const ListingItem& item = lst[lidx];
        if(item.type != LISTINGITEM_INSTRUCTION)
            continue;

        Byte b = mem->at(item.index);

        if(b.has(BF_JUMPDST)) {
            for(const auto& [fromidx, type] :
                ctx->get_refs_to_type(item.index, CR_JUMP)) {
                const Segment* seg = state::context->index_to_segment(fromidx);

                if(seg && (seg->type & SEG_HASCODE)) {
                    this->insert_path(mem->at(fromidx),
                                      this->calculate_index(fromidx), i);
                }
            }
        }
        else if(b.has(BF_JUMP)) {
            const auto& mem = state::context->memory;

            for(Database::Ref r :
                ctx->get_refs_from_type(item.index, CR_JUMP)) {
                const Segment* seg = state::context->index_to_segment(r.index);

                if(seg && (seg->type & SEG_HASCODE) &&
                   mem->at(r.index).has(BF_CODE)) {
                    this->insert_path(b, i, this->calculate_index(r.index));
                }
            }
        }
    }

    return m_path;
}

void Surface::set_rdil(bool v) {
    if(v)
        m_renderer->flags |= SURFACE_RDIL;
    else
        m_renderer->flags &= ~SURFACE_RDIL;
}

void Surface::set_columns(usize cols) { m_renderer->columns = cols; }

void Surface::set_position(int row, int col) {
    if(row == m_selrow && col == m_selcol)
        return;

    this->update_history(m_histback);

    if(row >= 0)
        m_selrow = row;
    if(col >= 0)
        m_selcol = col;

    this->fit(m_selrow, m_selcol);
    this->select(m_selrow, m_selcol);
}

bool Surface::select_word(int row, int col) {
    if(row >= this->rows_count())
        return false;

    const SurfaceRow& sfrow = this->rows[row];

    if(col >= static_cast<int>(sfrow.cells.size()))
        col = sfrow.cells.size() - 1;

    if(Renderer::is_char_skippable(sfrow.cells[col].ch))
        return false;

    usize startcol = 0, endcol = 0;

    for(int i = col; i-- > 0;) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(Renderer::is_char_skippable(cell.ch)) {
            startcol = i + 1;
            break;
        }
    }

    for(int i = col; i < static_cast<int>(sfrow.cells.size()); i++) {
        RDSurfaceCell cell = sfrow.cells[i];
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

bool Surface::select(int row, int col) {
    if(row == m_row && col == m_col)
        return false;

    if(row >= 0)
        m_row = row;
    if(col >= 0)
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

    for(int i = startrow; i < this->rows_count() && i <= endrow; i++) {
        if(!m_strcache.empty())
            m_strcache += "\n";

        const SurfaceRow& sfrow = this->rows[i];
        usize s = 0, e = 0;

        if(!sfrow.cells.empty())
            e = sfrow.cells.size() - 1;

        if(i == startrow)
            s = startcol;
        if(i == endrow)
            e = endcol;

        for(auto j = s; j <= e; j++)
            m_strcache.append(&sfrow.cells[j].ch, 1);
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

        for(RDSurfaceCell col : row.cells)
            m_strcache.append(&col.ch, 1);
    }

    return m_strcache;
}

bool Surface::has_selection() const {
    return (m_row != m_selrow) || (m_col != m_selcol);
}

bool Surface::has_rdil() const { return m_renderer->has_flag(SURFACE_RDIL); }

const ListingItem& Surface::get_listing_item(const SurfaceRow& sfrow) const {
    assume(sfrow.listingindex < state::context->listing.size());
    return state::context->listing[sfrow.listingindex];
}

int Surface::calculate_index(usize idx) const {
    if(!this->rows.empty()) {
        const ListingItem& first = this->get_listing_item(this->rows.front());
        if(idx < first.index)
            return -1;

        const ListingItem& last = this->get_listing_item(this->rows.back());
        if(idx > last.index)
            return this->rows.size() + 1;
    }

    int residx = -1;

    for(usize i = this->rows.size(); i-- > 0;) {
        const ListingItem& item = this->get_listing_item(this->rows[i]);

        if(item.index == idx) {
            residx = i;

            if(item.type == LISTINGITEM_INSTRUCTION)
                break;
        }

        if(item.index < idx)
            break;
    }

    return residx;
}

void Surface::update_history(History& history) const {
    if(!this->start || m_lockhistory)
        return;

    RDSurfacePosition pos = this->end_selection();

    HistoryItem hitem = {
        .start = *this->start,
        .row = pos.row,
        .col = pos.col,
    };

    if(history.empty() || history.front() != hitem)
        history.emplace_front(hitem);
}

void Surface::insert_path(Byte b, int fromrow, int torow) const {
    if(fromrow == torow)
        return;

    if(auto it = m_done.emplace(fromrow, torow); !it.second)
        return;

    if(fromrow > torow) { // Loop
        if(b.has(BF_FLOW)) {
            m_path.push_back(
                RDSurfacePath{fromrow, torow, THEME_GRAPHEDGELOOPCOND});
        }
        else {
            m_path.push_back(
                RDSurfacePath{fromrow, torow, THEME_GRAPHEDGELOOP});
        }
    }
    else {
        if(b.has(BF_FLOW)) {
            m_path.push_back(RDSurfacePath{fromrow, torow, THEME_SUCCESS});
        }
        else {
            m_path.push_back(RDSurfacePath{fromrow, torow, THEME_GRAPHEDGE});
        }
    }
}

void Surface::render_finalize() {
    m_renderer->fill_columns();
    m_renderer->highlight_row(m_row);

    if(!this->has_selection())
        m_renderer->highlight_words(m_row, m_col);

    auto startsel = this->start_selection(), endsel = this->end_selection();
    m_renderer->highlight_selection(startsel, endsel);

    m_renderer->highlight_cursor(m_row, m_col);
    m_renderer->swap(this->rows);
}

void Surface::render_range(LIndex start, usize n) {
    const Listing& listing = state::context->listing;

    auto it = std::next(listing.begin(), start);
    if(it == listing.end())
        return;

    for(usize i = 0; it != listing.end() && i < n; it++, i++) {
        m_renderer->set_current_item(start + i, *it);

        switch(it->type) {
            case LISTINGITEM_EMPTY: m_renderer->new_row(*it); break;
            case LISTINGITEM_HEX_DUMP: this->render_hexdump(*it); break;
            case LISTINGITEM_FILL: this->render_fill(*it); break;
            case LISTINGITEM_SEGMENT: this->render_segment(*it); break;
            case LISTINGITEM_FUNCTION: this->render_function(*it); break;
            case LISTINGITEM_LABEL: this->render_label(*it); break;

            case LISTINGITEM_INSTRUCTION: {
                m_renderer->new_row(*it);

                if(this->has_rdil())
                    m_renderer->rdil();
                else
                    m_renderer->instr();

                if(state::context->memory->at(it->index).has(BF_REFSFROM))
                    this->render_refs(*it);

                this->render_comment(*it);
                break;
            }

            case LISTINGITEM_TYPE:
                if(it->dtype->n > 0)
                    this->render_array(*it);
                else
                    this->render_type(*it);

                if(state::context->memory->at(it->index).has(BF_REFSFROM))
                    this->render_refs(*it);

                this->render_comment(*it);
                break;

            default: break;
        }
    }
}

void Surface::render_hexdump(const ListingItem& item) {
    static constexpr usize HEX_WIDTH = 16;

    const auto& mem = state::context->memory;
    usize c = 0;
    m_renderer->new_row(item);

    for(usize i = item.start_index; i < item.end_index; i++, c += 3) {
        Byte b = mem->at(i);

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
        Byte b = mem->at(i);

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

void Surface::render_fill(const ListingItem& item) {
    const Context* ctx = state::context;
    Byte b = ctx->memory->at(item.index);

    m_renderer->new_row(item).chunk(".fill", THEME_FUNCTION).ws();

    if(b.has_byte())
        m_renderer->chunk(ctx->to_hex(b.byte(), 2), THEME_CONSTANT);
    else
        m_renderer->chunk("??", THEME_NOP);

    m_renderer->ws()
        .chunk("(")
        .constant(item.end_index - item.start_index, 16)
        .chunk(")");
}

void Surface::render_label(const ListingItem& item) {
    std::string name = state::context->get_name(item.index);
    m_renderer->new_row(item).chunk(name + ":", THEME_ADDRESS);
}

void Surface::render_segment(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOSEGMENT))
        return;

    m_renderer->new_row(item);

    const RDProcessor* p = state::context->processor;
    assume(p);

    const Segment* s = state::context->index_to_segment(item.index);
    assume(s);

    RDSegment cs = api::to_c(*s);

    if(p->rendersegment)
        p->rendersegment(p, api::to_c(m_renderer.get()), &cs);
    else
        builtins::processor::render_segment(p, api::to_c(m_renderer.get()),
                                            &cs);
}

void Surface::render_function(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOFUNCTION))
        return;

    m_renderer->new_row(item);

    const RDProcessor* p = state::context->processor;
    assume(p);

    const Function* f = state::context->index_to_function(item.index);
    assume(f);

    if(p->renderfunction)
        p->renderfunction(p, api::to_c(m_renderer.get()), api::to_c(f));
    else
        builtins::processor::render_function(p, api::to_c(m_renderer.get()),
                                             api::to_c(f));
}

void Surface::render_type(const ListingItem& item) {
    assume(item.dtype_context);
    assume(item.dtype);

    auto type = item.dtype_context;
    assume(type);
    const Context* ctx = state::context;
    const typing::TypeDef* td = ctx->types.get_typedef(*type);
    const auto& mem = ctx->memory;
    std::string fname;

    if(item.field_index) {
        assume(*item.field_index < td->dict.size());
        auto f = td->dict.at(*item.field_index);
        fname = f.second;
        type = item.dtype;
        td = ctx->types.get_typedef(*type);
    }
    else
        fname = ctx->get_name(item.index);

    assume(type);
    std::string t = ctx->types.to_string(*type);

    m_renderer->new_row(item);

    if(item.array_index)
        m_renderer->arr_index(*item.array_index);

    if(td->is_struct()) {
        if(!item.array_index) {
            std::string label = ctx->get_name(item.index);
            m_renderer->function("struct").ws().type(td->name).ws().chunk(
                label);
        }

        m_renderer->word("=");
        return;
    }

    if(!item.array_index)
        m_renderer->type(t).ws().chunk(fname);

    switch(td->get_id()) {
        case typing::ids::CHAR:
        case typing::ids::WCHAR: {
            tl::optional<char> ch;

            if(td->get_id() == typing::ids::WCHAR)
                ch = mem->get_wchar(item.index);
            else
                ch = mem->get_char(item.index);

            if(ch.has_value())
                m_renderer->word("=").quote(std::string_view{&ch.value(), 1},
                                            "\'");
            else
                m_renderer->word("=").unknown();
            break;
        }

        case typing::ids::I8:
        case typing::ids::U8:
        case typing::ids::I16:
        case typing::ids::U16:
        case typing::ids::I32:
        case typing::ids::U32:
        case typing::ids::I64:
        case typing::ids::U64:
        case typing::ids::I16BE:
        case typing::ids::U16BE:
        case typing::ids::I32BE:
        case typing::ids::U32BE:
        case typing::ids::I64BE:
        case typing::ids::U64BE: {
            mem->get_type(item.index, td->to_type())
                .map([&](const typing::Value& v) {
                    bool isaddr = false;
                    std::string vs = surface_valuestr(v, isaddr);
                    m_renderer->word("=").chunk(vs, isaddr ? THEME_ADDRESS
                                                           : THEME_CONSTANT);
                })
                .or_else([&]() { m_renderer->word("=").unknown(); });
            break;
        }

        case typing::ids::STR:
        case typing::ids::WSTR: {
            tl::optional<std::string> v;

            if(td->get_id() == typing::ids::WSTR)
                v = state::context->memory->get_wstr(item.index);
            else
                v = state::context->memory->get_str(item.index);

            if(v.has_value())
                m_renderer->ws().string(*v).chunk(",").constant(0);
            else
                m_renderer->ws().unknown();
            break;
        }

        default: unreachable; break;
    }
}

void Surface::render_comment(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOCOMMENTS))
        return;

    if(Byte b = state::context->memory->at(item.index); !b.has(BF_COMMENT))
        return;

    m_renderer->ws(8);
    int i = 0;

    utils::split_each(
        state::context->get_comment(item.index), '\n', [&](std::string_view x) {
            m_renderer->comment(i++ > 0 ? " | " : "# ").comment(x);
            return true;
        });
}

void Surface::render_refs(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOREFS))
        return;

    const Context* ctx = state::context;
    const auto& mem = ctx->memory;
    bool paddingdone = false;

    for(const auto& [index, _] : ctx->get_refs_from(item.index)) {
        if(!mem->at(index).has(BF_TYPE))
            continue;

        if(!paddingdone) {
            m_renderer->ws(COLUMN_PADDING);
            paddingdone = true;
        }

        auto type = ctx->get_type(index);
        assume(type);
        const typing::TypeDef* td = ctx->types.get_typedef(*type);
        assume(td);

        switch(td->get_id()) {
            case typing::ids::CHAR: {
                if(type->n > 0) {
                    mem->get_str(index, type->n).map([&](const std::string& x) {
                        m_renderer->string(x);
                    });
                }
                else {
                    mem->get_str(index, 1).map(
                        [&](const std::string& x) { m_renderer->string(x); });
                }
                break;
            }

            case typing::ids::WCHAR: {
                if(type->n > 0) {
                    mem->get_wstr(index, type->n)
                        .map([&](const std::string& x) {
                            m_renderer->string(x);
                        });
                }
                else {
                    mem->get_wstr(index, 1).map(
                        [&](const std::string& x) { m_renderer->string(x); });
                }
                break;
            }

            case typing::ids::STR: {
                mem->get_str(index).map(
                    [&](const std::string& x) { m_renderer->string(x); });
                break;
            }

            case typing::ids::WSTR: {
                mem->get_wstr(index).map(
                    [&](const std::string& x) { m_renderer->string(x); });
                break;
            }

            default: break;
        }
    }
}

void Surface::render_array(const ListingItem& item) {
    assume(item.dtype_context);
    assume(item.dtype);

    auto type = item.dtype;
    std::string chars;

    const typing::TypeDef* td = state::context->types.get_typedef(*type);
    assume(td);

    if(td->get_id() == typing::ids::CHAR ||
       td->get_id() == typing::ids::WCHAR) {
        const auto& mem = state::context->memory;
        usize idx = item.index;

        for(usize i = 0; i < type->n && idx < mem->size();
            i++, idx += td->size) {
            auto b = mem->get_type(idx, td->to_type());

            if(!b) {
                chars += "\",?\"";
                continue;
            }

            if(std::isprint(b->ch_v))
                chars += b->ch_v;
            else
                chars += fmt::format("\\x{}", static_cast<int>(b->ch_v));
        }
    }

    if(item.field_index) {
        auto ptc = item.dtype_context;
        assume(ptc);
        const typing::TypeDef* ptd = state::context->types.get_typedef(*ptc);

        assume(*item.field_index < ptd->dict.size());
        auto field = ptd->dict[*item.field_index];

        m_renderer->new_row(item);

        if(td->is_struct())
            m_renderer->function("struct ");

        m_renderer->type(state::context->types.to_string(*type))
            .ws()
            .chunk(field.second)
            .word("=");

        if(!chars.empty())
            m_renderer->string(chars);
    }
    else {
        m_renderer->new_row(item);

        if(td->is_struct())
            m_renderer->function("struct ");

        std::string name = state::context->get_name(item.index);
        m_renderer->type(state::context->types.to_string(*type))
            .ws()
            .chunk(name);

        if(item.array_index)
            m_renderer->arr_index(*item.array_index);

        m_renderer->word("=");

        if(!chars.empty())
            m_renderer->string(chars);
    }
}

void Surface::fit(int& row, int& col) {
    if(this->rows.empty()) {
        row = 0;
        col = 0;
    }
    else {
        if(row >= this->rows_count())
            row = this->rows_count() - 1;

        int ncols = this->cols_count(row);

        if(!ncols)
            col = 0;
        else if(col >= ncols)
            col = ncols - 1;
    }
}

int Surface::index_of(MIndex index) const {
    for(usize i = 0; i < this->rows.size(); i++) {
        const SurfaceRow& sfrow = this->rows[i];
        const ListingItem& item = this->get_listing_item(sfrow);
        if(item.index == index)
            return static_cast<int>(i);
    }

    return -1;
}

int Surface::last_index_of(MIndex index) const {
    for(usize i = this->rows.size(); i-- > 0;) {
        const SurfaceRow& sfrow = this->rows[i];
        const ListingItem& item = this->get_listing_item(sfrow);
        if(item.index == index)
            return static_cast<int>(i);
    }

    return -1;
}

void Surface::clear_selection() {
    m_selrow = m_row;
    m_selcol = m_col;
}

void Surface::clear_history() {
    m_histback.clear();
    m_histforward.clear();
}

} // namespace redasm
