#include "listing.h"
#include "context.h"
#include "error.h"
#include "state.h"
#include <algorithm>
#include <utility>

namespace redasm {

Listing::ConstIterator Listing::lower_bound(MIndex idx) const {
    auto it = std::lower_bound(
        m_items.begin(), m_items.end(), idx,
        [](const ListingItem& x, MIndex index) { return x.index < index; });

    if(it != m_items.end() && it->index == idx)
        return it;

    return m_items.end();
};

Listing::ConstIterator Listing::upper_bound(MIndex idx,
                                            ConstIterator begin) const {
    return std::upper_bound(
        std::move(begin), m_items.end(), idx,
        [](MIndex index, const ListingItem& x) { return index < x.index; });
}

void Listing::clear() {
    m_symbols.clear();
    m_imports.clear();
    m_exports.clear();
    m_items.clear();
    m_fieldindex.clear();
    m_currtype.clear();
    m_indent = 0;
}

void Listing::push_indent(int c) {
    while(c-- > 0)
        m_indent += INDENT;
}

void Listing::pop_indent(int c) {
    while(c-- > 0) {
        if(m_indent >= INDENT)
            m_indent -= INDENT;
        else
            break;
    }
}

tl::optional<usize> Listing::field_index() const {
    if(m_fieldindex.empty())
        return tl::nullopt;
    return m_fieldindex.back();
}

tl::optional<RDType> Listing::current_type() const {
    if(m_currtype.empty())
        return tl::nullopt;
    return m_currtype.back();
}

void Listing::push_fieldindex(usize arg) { m_fieldindex.push_back(arg); }

void Listing::pop_fieldindex() {
    assume(!m_fieldindex.empty());
    m_fieldindex.pop_back();
}

void Listing::push_type(RDType t) { m_currtype.push_back(t); }

void Listing::pop_type() {
    assume(!m_currtype.empty());
    m_currtype.pop_back();
}

usize Listing::type(MIndex index, RDType t) {
    usize lidx = this->push_item(LISTINGITEM_TYPE, index);
    m_items[lidx].dtype = t;

    if(!m_items[lidx].dtype_context)
        m_items[lidx].dtype_context = m_items[lidx].dtype;

    if(!this->field_index() && !this->current_type())
        m_symbols.push_back(lidx);

    this->check_flags(lidx, index);
    return lidx;
}

usize Listing::instruction(MIndex index) {
    return this->push_item(LISTINGITEM_INSTRUCTION, index);
}

usize Listing::label(MIndex index) {
    return this->push_item(LISTINGITEM_LABEL, index);
}

usize Listing::function(MIndex index) {
    usize lidx = this->push_item(LISTINGITEM_FUNCTION, index);
    m_symbols.push_back(lidx);
    this->check_flags(lidx, index);
    return lidx;
}

usize Listing::segment(MIndex index) {
    usize idx = this->push_item(LISTINGITEM_SEGMENT, index);
    m_symbols.push_back(idx);
    m_currentsegment = state::context->index_to_segment(index);
    return idx;
}

void Listing::hex_dump(MIndex startindex, MIndex endindex) {
    usize lidx = this->push_item(LISTINGITEM_HEX_DUMP, startindex);
    m_items[lidx].end_index = endindex;
}

void Listing::fill(MIndex startindex, MIndex endindex) {
    usize lidx = this->push_item(LISTINGITEM_FILL, startindex);
    m_items[lidx].end_index = endindex;
}

usize Listing::push_item(RDListingItemType type, MIndex index) {
    usize idx = m_items.size();

    ListingItem li{};
    li.type = type;
    li.index = index;
    li.indent = m_indent;
    li.field_index = this->field_index();
    li.dtype_context = this->current_type();

    m_items.push_back(li);
    return idx;
}

void Listing::check_flags(LIndex listingidx, MIndex index) {
    Byte b = state::context->memory->at(index);

    if(b.has(BF_IMPORT))
        m_imports.push_back(listingidx);
    else if(b.has(BF_EXPORT))
        m_exports.push_back(listingidx);
}

} // namespace redasm
