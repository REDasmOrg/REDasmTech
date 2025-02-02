#include "listing.h"
#include "error.h"
#include "memory/memory.h"
#include <algorithm>
#include <utility>

namespace redasm {

Listing::ConstIterator Listing::lower_bound(RDAddress address,
                                            ConstIterator begin) const {
    auto it = std::lower_bound(
        std::move(begin), m_items.end(), address,
        [](const ListingItem& x, RDAddress addr) { return x.address < addr; });

    if(it != m_items.end() && it->address == address) return it;
    return m_items.end();
};

Listing::ConstIterator Listing::upper_bound(RDAddress address,
                                            ConstIterator begin) const {
    return std::upper_bound(
        std::move(begin), m_items.end(), address,
        [](RDAddress addr, const ListingItem& x) { return addr < x.address; });
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
    if(m_fieldindex.empty()) return tl::nullopt;
    return m_fieldindex.back();
}

tl::optional<RDType> Listing::current_type() const {
    if(m_currtype.empty()) return tl::nullopt;
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

usize Listing::type(RDAddress address, RDType t) {
    usize lidx = this->push_item(LISTINGITEM_TYPE, address);
    m_items[lidx].dtype = t;

    if(!m_items[lidx].dtype_context)
        m_items[lidx].dtype_context = m_items[lidx].dtype;

    if(!this->field_index() && !this->current_type()) m_symbols.push_back(lidx);

    this->check_flags(lidx, address);
    return lidx;
}

usize Listing::instruction(RDAddress address) {
    return this->push_item(LISTINGITEM_INSTRUCTION, address);
}

usize Listing::label(RDAddress address) {
    return this->push_item(LISTINGITEM_LABEL, address);
}

usize Listing::function(RDAddress address) {
    usize lidx = this->push_item(LISTINGITEM_FUNCTION, address);
    m_symbols.push_back(lidx);
    this->check_flags(lidx, address);
    return lidx;
}

usize Listing::segment(const RDSegmentNew* seg) {
    usize idx = this->push_item(LISTINGITEM_SEGMENT, seg->start);
    m_symbols.push_back(seg->start);
    m_currentsegment = seg;
    return idx;
}

void Listing::hex_dump(RDAddress startaddr, RDAddress endaddr) {
    usize lidx = this->push_item(LISTINGITEM_HEX_DUMP, startaddr);
    m_items[lidx].end_address = endaddr;
}

void Listing::fill(RDAddress startaddr, RDAddress endaddr) {
    usize lidx = this->push_item(LISTINGITEM_FILL, startaddr);
    m_items[lidx].end_address = endaddr;
}

usize Listing::push_item(RDListingItemType type, RDAddress address) {
    usize idx = m_items.size();

    ListingItem li{};
    li.type = type;
    li.address = address;
    li.indent = m_indent;
    li.field_index = this->field_index();
    li.dtype_context = this->current_type();

    m_items.push_back(li);
    return idx;
}

void Listing::check_flags(LIndex listingidx, RDAddress address) {
    assume(m_currentsegment);

    if(memory::has_flag(m_currentsegment, address, BF_IMPORT))
        m_imports.push_back(listingidx);
    else if(memory::has_flag(m_currentsegment, address, BF_EXPORT))
        m_exports.push_back(listingidx);
}

} // namespace redasm
