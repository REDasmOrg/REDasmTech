#pragma once

#include "segment.h"
#include <deque>
#include <redasm/listing.h>
#include <redasm/types.h>
#include <redasm/typing.h>
#include <tl/optional.hpp>
#include <vector>

namespace redasm {

struct ListingItem {
    RDListingItemType type;

    union {
        MIndex start_index;
        MIndex index;
    };

    MIndex end_index;
    usize indent;

    tl::optional<RDType> dtype_context;
    tl::optional<RDType> dtype;

    tl::optional<usize> array_index;
    tl::optional<usize> field_index;
};

class Listing {
private:
    using Type = std::vector<ListingItem>;
    using ValueType = typename Type::value_type;
    using Iterator = typename Type::iterator;
    using ConstIterator = typename Type::const_iterator;

    static constexpr usize INDENT = 2;

public:
    using IndexList = std::vector<usize>;

    auto cbegin() const { return m_items.cbegin(); }
    auto cend() const { return m_items.cend(); }
    auto begin() { return m_items.begin(); }
    auto end() { return m_items.end(); }
    auto begin() const { return m_items.begin(); }
    auto end() const { return m_items.end(); }
    auto size() const { return m_items.size(); }
    auto empty() const { return m_items.empty(); }
    auto& front() { return m_items.front(); }
    auto& back() { return m_items.back(); }
    const auto& front() const { return m_items.front(); }
    const auto& back() const { return m_items.back(); }
    ValueType& operator[](LIndex idx) { return m_items.at(idx); }
    const ValueType& operator[](LIndex idx) const { return m_items.at(idx); }

    ConstIterator upper_bound(MIndex idx) const {
        return this->upper_bound(idx, m_items.begin());
    }

    const IndexList& symbols() const { return m_symbols; }
    const IndexList& imports() const { return m_imports; }
    const IndexList& exports() const { return m_exports; }

    ConstIterator lower_bound(MIndex idx) const;
    ConstIterator upper_bound(MIndex idx, ConstIterator begin) const;
    void hex_dump(MIndex startindex, MIndex endindex);
    void fill(MIndex index, MIndex endindex);
    usize type(MIndex index, RDType t);
    usize instruction(MIndex index);
    usize label(MIndex index);
    usize function(MIndex index);
    usize segment(MIndex index);

public: // State management functions
    tl::optional<usize> field_index() const;
    tl::optional<RDType> current_type() const;
    const Segment* current_segment() const { return m_currentsegment; }
    void clear();
    void push_indent(int c = 1);
    void pop_indent(int c = 1);
    void push_fieldindex(usize arg);
    void pop_fieldindex();
    void push_type(RDType t);
    void pop_type();

private:
    usize push_item(RDListingItemType type, MIndex index);
    void check_flags(LIndex listingidx, MIndex index);

private:
    std::deque<usize> m_fieldindex;
    std::deque<RDType> m_currtype;
    const Segment* m_currentsegment{nullptr};
    IndexList m_symbols, m_exports, m_imports;
    usize m_indent{0};
    Type m_items;
};

} // namespace redasm
