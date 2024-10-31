#include "surface.h"

#include "../api/marshal.h"
#include "../builtins/processor.h"
#include "../context.h"
#include "../state.h"
#include <algorithm>

namespace redasm {

namespace {

constexpr usize COLUMN_PADDING = 6;

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

Surface::Surface(usize flags) {
    m_renderer = std::make_unique<Renderer>(flags);

    this->lock_history([&]() { this->jump_to_ep(); });
}

tl::optional<MIndex> Surface::current_index() const {
    if(!this->start || state::context->listing.empty())
        return tl::nullopt;

    LIndex idx =
        std::min(*this->start + m_row, state::context->listing.size() - 1);
    return state::context->listing[idx].index;
}

tl::optional<usize> Surface::index_under_cursor() const {
    if(this->start) {
        if(std::string w = m_renderer->word_at(m_row, m_col); !w.empty())
            return state::context->database.get_index(w);
    }

    return tl::nullopt;
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

    // if(!this->jump_to(f.entry))
    //     return;

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
    this->lock_history(
        [&]() { res = this->jump_to(state::context->entrypoint); });
    return res;
}

const std::vector<RDSurfacePath>& Surface::get_path() const {
    const Listing& lst = state::context->listing;
    const Database& db = state::context->database;
    const auto& mem = state::context->memory;

    m_path.clear();
    m_done.clear();

    for(usize i = 0; this->start && i < this->rows.size(); i++) {
        usize lidx = *this->start + i;
        if(lidx >= lst.size())
            break;

        const ListingItem& item = lst[lidx];
        if(item.type != ListingItemType::INSTRUCTION)
            continue;

        Byte b = mem->at(item.index);

        if(b.has(BF_JUMPDST)) {
            const AddressDetail& d = db.get_detail(item.index);

            for(const auto& [fromidx, cr] : d.refs) {
                if(cr != CR_JUMP)
                    continue;

                const Segment* seg = state::context->index_to_segment(fromidx);

                if(seg && seg->type & SEGMENTTYPE_HASCODE) {
                    this->insert_path(mem->at(fromidx),
                                      this->calculate_index(fromidx), i);
                }
            }
        }
        else if(b.has(BF_JUMP)) {
            const AddressDetail& d = db.get_detail(item.index);

            for(usize toidx : d.jumps) {
                const Segment* seg = state::context->index_to_segment(toidx);
                if(seg && seg->type & SEGMENTTYPE_HASCODE)
                    this->insert_path(b, i, this->calculate_index(toidx));
            }
        }
    }

    return m_path;
}

void Surface::set_columns(usize cols) { m_renderer->columns = cols; }

void Surface::set_position(usize row, usize col) {
    if(row == m_selrow && col == m_selcol)
        return;

    this->update_history(m_histback);

    m_selrow = row;
    m_selcol = col;
    this->fit(m_selrow, m_selcol);
    this->select(m_selrow, m_selcol);
}

bool Surface::select_word(usize row, usize col) {
    if(row >= this->rows.size())
        return false;

    const SurfaceRow& sfrow = this->rows[row];

    if(col >= sfrow.cells.size())
        col = sfrow.cells.size() - 1;

    if(Renderer::is_char_skippable(sfrow.cells[col].ch))
        return false;

    usize startcol = 0, endcol = 0;

    for(usize i = col; i-- > 0;) {
        RDSurfaceCell cell = sfrow.cells[i];
        if(Renderer::is_char_skippable(cell.ch)) {
            startcol = i + 1;
            break;
        }
    }

    for(usize i = col; i < sfrow.cells.size(); i++) {
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

RDRendererParams Surface::create_render_params(const ListingItem& item) const {
    RDRendererParams rp{};
    rp.renderer = api::to_c(m_renderer.get());
    rp.address = m_renderer->current_address();
    rp.byte = state::context->memory->at(item.index).value;
    return rp;
}

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

            if(item.type == ListingItemType::INSTRUCTION)
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
        *this->start,
        pos.row,
        pos.col,
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
            case ListingItemType::EMPTY: m_renderer->new_row(*it); break;
            case ListingItemType::HEX_DUMP: this->render_hexdump(*it); break;
            case ListingItemType::SEGMENT: this->render_segment(*it); break;
            case ListingItemType::FUNCTION: this->render_function(*it); break;
            case ListingItemType::JUMP: this->render_jump(*it); break;
            case ListingItemType::INSTRUCTION: this->render_instr(*it); break;
            case ListingItemType::TYPE: this->render_type(*it); break;
            case ListingItemType::ARRAY: this->render_array(*it); break;
            default: break;
        }

        if(state::context->memory->at(it->index).has(BF_REFSTO))
            this->render_refs(*it);
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

void Surface::render_jump(const ListingItem& item) {
    std::string name = state::context->get_name(item.index);
    m_renderer->new_row(item).chunk(name + ":", THEME_ADDRESS);
}

void Surface::render_segment(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOSEGMENT))
        return;

    m_renderer->new_row(item);

    const RDProcessor* p = state::context->processor;
    assume(p);

    RDRendererParams rp = this->create_render_params(item);

    if(!p->rendersegment || !p->rendersegment(p, &rp))
        assume(builtins::processor::render_segment(&rp));
}

void Surface::render_function(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOFUNCTION))
        return;

    m_renderer->new_row(item);

    const RDProcessor* p = state::context->processor;
    assume(p);

    RDRendererParams rp = this->create_render_params(item);

    if(!p->renderfunction || !p->renderfunction(p, &rp))
        assume(builtins::processor::render_function(&rp));
}

void Surface::render_type(const ListingItem& item) {
    assume(item.parsed_type_context);
    assume(item.parsed_type);

    tl::optional<typing::ParsedType> pt = item.parsed_type_context;
    const auto& mem = state::context->memory;
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

        case typing::types::I8:
        case typing::types::U8: {
            auto v = mem->get_u8(item.index);

            if(v.has_value()) {

                auto [isptr, val] = surface_checkpointer(*pt, *v);
                m_renderer->word("=").chunk(val, isptr ? THEME_ADDRESS
                                                       : THEME_CONSTANT);
            }
            else
                m_renderer->word("=").unknown();
            break;
        }

        case typing::types::I16:
        case typing::types::U16: {
            auto v = mem->get_u16(item.index, pt->type->is_big());

            if(v.has_value()) {
                auto [isptr, val] = surface_checkpointer(*pt, *v);

                m_renderer->word("=").chunk(val, isptr ? THEME_ADDRESS
                                                       : THEME_CONSTANT);
            }
            else
                m_renderer->word("=").unknown();
            break;
        }

        case typing::types::I32:
        case typing::types::U32: {
            auto v = mem->get_u32(item.index, pt->type->is_big());

            if(v.has_value()) {
                auto [isptr, val] = surface_checkpointer(*pt, *v);
                m_renderer->word("=").chunk(val, isptr ? THEME_ADDRESS
                                                       : THEME_CONSTANT);
            }
            else
                m_renderer->word("=").unknown();
            break;
        }

        case typing::types::I64:
        case typing::types::U64: {
            auto v = mem->get_u64(item.index, pt->type->is_big());
            assume(v.has_value());
            if(v.has_value())
                m_renderer->word("=").constant(state::context->to_hex(*v, 16));
            else
                m_renderer->word("=").unknown();
            break;
        }

        case typing::types::STR:
        case typing::types::WSTR: {
            tl::optional<std::string> v;

            if(pt->type->is_wide())
                v = state::context->memory->get_wstring(item.index);
            else
                v = state::context->memory->get_string(item.index);

            if(v.has_value())
                m_renderer->ws().string(*v).chunk(",").constant("0");
            else
                m_renderer->ws().unknown();
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

void Surface::render_instr(const ListingItem& item) {
    m_renderer->new_row(item);

    RDRendererParams rp = this->create_render_params(item);
    const RDProcessor* p = state::context->processor;
    assume(p);

    if(p->renderinstruction && p->renderinstruction(p, &rp)) {
        if(Byte b = state::context->memory->at(item.index); !b.has(BF_REFS))
            return;
    }
    else
        m_renderer->chunk("???");
}

void Surface::render_comments(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOCOMMENTS))
        return;
}

void Surface::render_refs(const ListingItem& item) {
    if(m_renderer->has_flag(SURFACE_NOREFS))
        return;

    const Context* ctx = state::context;
    const auto& mem = ctx->memory;
    const AddressDetail& d = state::context->database.get_detail(item.index);

    m_renderer->ws(COLUMN_PADDING);

    for(const auto& [index, _] : d.refsto) {
        if(!mem->at(index).has(BF_TYPE))
            continue;

        const AddressDetail& d = ctx->database.get_detail(index);
        auto pt = ctx->types.parse(d.type_name);
        assume(pt.has_value());

        if(!pt->type->is_str() && !pt->type->is_char())
            continue;

        if(pt->type->is_wide()) {
            mem->get_wstring(index).map(
                [&](const std::string& x) { m_renderer->string(x); });
        }
        else {
            mem->get_string(index).map(
                [&](const std::string& x) { m_renderer->string(x); });
        }
    }
}

void Surface::render_array(const ListingItem& item) {
    assume(item.parsed_type);

    auto pt = *item.parsed_type;
    std::string chars;

    if(pt.type->is_char()) {
        const auto& mem = state::context->memory;
        usize idx = item.index;

        for(usize i = 0; i < pt.n && idx < mem->size();
            i++, idx += pt.type->size) {
            auto b = mem->get_type(idx, pt.type->name);

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
        auto ptc = *item.parsed_type_context;
        assume(*item.field_index < ptc.type->dict.size());
        auto field = ptc.type->dict[*item.field_index];

        m_renderer->new_row(item);

        if(pt.type->id() == typing::types::STRUCT)
            m_renderer->function("struct ");

        m_renderer->type(state::context->types.to_string(pt))
            .ws()
            .chunk(field.second)
            .word("=");

        if(!chars.empty())
            m_renderer->string(chars);
    }
    else {
        m_renderer->new_row(item);

        if(pt.type->id() == typing::types::STRUCT)
            m_renderer->function("struct ");

        std::string name = state::context->get_name(item.index);
        m_renderer->type(state::context->types.to_string(pt)).ws().chunk(name);

        if(item.array_index)
            m_renderer->arr_index(*item.array_index);

        m_renderer->word("=");

        if(!chars.empty())
            m_renderer->string(chars, false);
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

        if(this->rows[row].cells.empty())
            col = 0;
        else if(col >= this->rows[row].cells.size())
            col = this->rows[row].cells.size() - 1;
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
