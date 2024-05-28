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

const typing::Type* Listing::current_type() const {
    if(m_currtype.empty())
        return nullptr;
    return m_currtype.back();
}

void Listing::push_fieldindex(usize arg) { m_fieldindex.push_back(arg); }

void Listing::pop_fieldindex() {
    assume(!m_fieldindex.empty());
    m_fieldindex.pop_back();
}

void Listing::push_type(const typing::Type* type) {
    m_currtype.push_back(type);
}

void Listing::pop_type() {
    assume(!m_currtype.empty());
    m_currtype.pop_back();
}

usize Listing::type(usize index, std::string_view tname) {
    usize idx = this->push_item(ListingItemType::TYPE, index);
    m_items[idx].type_name.assign(tname.begin(), tname.end());

    if(m_items[idx].type_context.empty())
        m_items[idx].type_context = m_items[idx].type_name;

    if(!this->field_index() && !this->current_type())
        m_symbols.push_back(idx);

    return idx;
}

usize Listing::array(usize index, std::string_view tname, usize n) {
    usize idx = this->push_item(ListingItemType::ARRAY, index);
    m_items[idx].type_name.assign(tname.begin(), tname.end());
    m_items[idx].array_size = n;

    if(m_items[idx].type_context.empty())
        m_items[idx].type_context = m_items[idx].type_name;

    if(!this->field_index() && !this->current_type())
        m_symbols.push_back(idx);

    return idx;
}

usize Listing::code(usize index) {
    return this->push_item(ListingItemType::CODE, index);
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

    const typing::Type* t = this->current_type();

    if(t)
        li.type_context = t->name;

    m_items.push_back(li);
    return idx;
}

} // namespace redasm
