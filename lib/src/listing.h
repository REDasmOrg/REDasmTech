#pragma once

#include "typing/typing.h"
#include <deque>
#include <redasm/types.h>
#include <tl/optional.hpp>
#include <vector>

namespace redasm {

enum class ListingItemType {
    EMPTY = 0,
    HEX_DUMP,
    CODE,
    BRANCH,
    SEGMENT,
    FUNCTION,
    TYPE,
    ARRAY,
};

struct ListingItem {
    ListingItemType type;

    union {
        usize start_index;
        usize index;
    };

    usize end_index;
    usize indent;
    usize array_size;

    std::string type_context;
    std::string type_name;
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
    inline ValueType& operator[](usize idx) { return m_items.at(idx); }

    inline const ValueType& operator[](usize idx) const {
        return m_items.at(idx);
    }

    inline ConstIterator upper_bound(usize idx) const {
        return this->upper_bound(idx, m_items.begin());
    }

    inline const IndexList& symbols() const { return m_symbols; }

    ConstIterator lower_bound(usize idx) const;
    ConstIterator upper_bound(usize idx, ConstIterator begin) const;
    void hex_dump(usize startindex, usize endindex);
    usize type(usize index, std::string_view tname);
    usize array(usize index, std::string_view tname, usize n);
    void code(usize index);
    void branch(usize index);
    void function(usize index);
    void segment(usize index);

public: // State management functions
    tl::optional<usize> field_index() const;
    const typing::Type* current_type() const;
    void clear();
    void push_indent(int c = 1);
    void pop_indent(int c = 1);
    void push_fieldindex(usize arg);
    void pop_fieldindex();
    void push_type(const typing::Type* type);
    void pop_type();

private:
    usize push_item(ListingItemType type, usize index);

private:
    std::deque<usize> m_fieldindex;
    std::deque<const typing::Type*> m_currtype;
    IndexList m_symbols;
    usize m_indent{0};
    Type m_items;
};

} // namespace redasm
