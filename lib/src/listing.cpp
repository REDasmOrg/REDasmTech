#include "listing.h"
#include "error.h"
#include <algorithm>
#include <utility>

namespace redasm {

Listing::ConstIterator Listing::lower_bound(usize idx) const {
    auto it = std::lower_bound(
        m_items.begin(), m_items.end(), idx,
        [](const ListingItem& x, usize index) { return x.index < index; });

    if(it != m_items.end() && it->index == idx)
        return it;

    return m_items.end();
};

Listing::ConstIterator Listing::upper_bound(usize idx,
                                            ConstIterator begin) const {
    return std::upper_bound(
        std::move(begin), m_items.end(), idx,
        [](usize index, const ListingItem& x) { return index < x.index; });
}

void Listing::clear() {
    m_symbols.clear();
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

tl::optional<typing::ParsedType> Listing::current_type() const {
    if(m_currtype.empty())
        return tl::nullopt;
    return m_currtype.back();
}

void Listing::push_fieldindex(usize arg) { m_fieldindex.push_back(arg); }

void Listing::pop_fieldindex() {
    assume(!m_fieldindex.empty());
    m_fieldindex.pop_back();
}

void Listing::push_type(const typing::ParsedType& pt) {
    m_currtype.push_back(pt);
}

void Listing::pop_type() {
    assume(!m_currtype.empty());
    m_currtype.pop_back();
}

usize Listing::type(usize index, const typing::ParsedType& pt) {
    usize idx = this->push_item(ListingItemType::TYPE, index);
    m_items[idx].parsed_type = pt;

    if(!m_items[idx].parsed_type_context)
        m_items[idx].parsed_type_context = m_items[idx].parsed_type;

    if(!this->field_index() && !this->current_type())
        m_symbols.push_back(idx);

    return idx;
}

usize Listing::array(usize index, const typing::ParsedType& pt) {
    usize idx = this->push_item(ListingItemType::ARRAY, index);
    m_items[idx].parsed_type = pt;

    if(!m_items[idx].parsed_type_context)
        m_items[idx].parsed_type_context = m_items[idx].parsed_type;

    if(!this->field_index() && !this->current_type())
        m_symbols.push_back(idx);

    return idx;
}

void Listing::code(usize index) {
    this->push_item(ListingItemType::CODE, index);
}

void Listing::branch(usize index) {
    this->push_item(ListingItemType::BRANCH, index);
}

void Listing::function(usize index) {
    m_symbols.push_back(this->push_item(ListingItemType::FUNCTION, index));
}

void Listing::segment(usize index) {
    m_symbols.push_back(this->push_item(ListingItemType::SEGMENT, index));
}

void Listing::hex_dump(usize startindex, usize endindex) {
    usize lidx = this->push_item(ListingItemType::HEX_DUMP, startindex);
    m_items[lidx].end_index = endindex;
}

usize Listing::push_item(ListingItemType type, usize index) {
    usize idx = m_items.size();

    ListingItem li{};
    li.type = type;
    li.index = index;
    li.indent = m_indent;
    li.field_index = this->field_index();
    li.parsed_type_context = this->current_type();

    m_items.push_back(li);
    return idx;
}

} // namespace redasm
