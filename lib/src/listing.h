#pragma once

#include "segment.h"
#include "typing/typing.h"
#include <deque>
#include <redasm/types.h>
#include <tl/optional.hpp>
#include <vector>

namespace redasm {

enum class ListingItemType {
    EMPTY = 0,
    HEX_DUMP,
    INSTRUCTION,
    JUMP,
    SEGMENT,
    FUNCTION,
    TYPE,
    ARRAY,
};

struct ListingItem {
    ListingItemType type;

    union {
        MIndex start_index;
        MIndex index;
    };

    MIndex end_index;
    usize indent;

    tl::optional<typing::ParsedType> parsed_type_context;
    tl::optional<typing::ParsedType> parsed_type;

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

    inline auto cbegin() const { return m_items.cbegin(); }
    inline auto cend() const { return m_items.cend(); }
    inline auto begin() { return m_items.begin(); }
    inline auto end() { return m_items.end(); }
    inline auto begin() const { return m_items.begin(); }
    inline auto end() const { return m_items.end(); }
    inline auto size() const { return m_items.size(); }
    inline auto empty() const { return m_items.empty(); }
    inline auto& front() { return m_items.front(); }
    inline auto& back() { return m_items.back(); }
    inline const auto& front() const { return m_items.front(); }
    inline const auto& back() const { return m_items.back(); }
    inline ValueType& operator[](LIndex idx) { return m_items.at(idx); }

    inline const ValueType& operator[](LIndex idx) const {
        return m_items.at(idx);
    }

    inline ConstIterator upper_bound(MIndex idx) const {
        return this->upper_bound(idx, m_items.begin());
    }

    inline const IndexList& symbols() const { return m_symbols; }
    inline const IndexList& imports() const { return m_imports; }
    inline const IndexList& exports() const { return m_exports; }

    ConstIterator lower_bound(MIndex idx) const;
    ConstIterator upper_bound(MIndex idx, ConstIterator begin) const;
    void hex_dump(MIndex startindex, MIndex endindex);
    usize type(MIndex index, const typing::ParsedType& pt);
    usize array(MIndex index, const typing::ParsedType& pt);
    usize instruction(MIndex index);
    usize jump(MIndex index);
    usize function(MIndex index);
    usize segment(MIndex index);

public: // State management functions
    tl::optional<usize> field_index() const;
    tl::optional<typing::ParsedType> current_type() const;
    inline const Segment* current_segment() const { return m_currentsegment; }
    void clear();
    void push_indent(int c = 1);
    void pop_indent(int c = 1);
    void push_fieldindex(usize arg);
    void pop_fieldindex();
    void push_type(const typing::ParsedType& pt);
    void pop_type();

private:
    usize push_item(ListingItemType type, MIndex index);
    void check_flags(LIndex listingidx, MIndex index);

private:
    std::deque<usize> m_fieldindex;
    std::deque<typing::ParsedType> m_currtype;
    const Segment* m_currentsegment{nullptr};
    IndexList m_symbols, m_exports, m_imports;
    usize m_indent{0};
    Type m_items;
};

} // namespace redasm
