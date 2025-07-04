#pragma once

#include <deque>
#include <redasm/listing.h>
#include <redasm/segment.h>
#include <redasm/types.h>
#include <redasm/typing.h>
#include <tl/optional.hpp>
#include <vector>

namespace redasm {

struct ListingItem {
    RDListingItemType type;

    union {
        RDAddress start_address;
        RDAddress address;
    };

    union {
        RDAddress end_address;
        usize length;
    };

    usize indent;

    tl::optional<RDType> dtype_context;
    tl::optional<RDType> dtype;

    tl::optional<usize> array_index;
    tl::optional<usize> field_index;

    usize string_index;
    char string_terminator;
};

class Listing {
private:
    using Type = std::vector<ListingItem>;
    using ValueType = typename Type::value_type;
    using Iterator = typename Type::iterator;
    using ConstIterator = typename Type::const_iterator;

    static constexpr usize INDENT = 2;

public:
    using LIndexList = std::vector<LIndex>;

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

    ConstIterator upper_bound(RDAddress address) const {
        return this->upper_bound(address, m_items.begin());
    }

    ConstIterator lower_bound(RDAddress address) const {
        return this->lower_bound(address, m_items.begin());
    }

    const LIndexList& symbols() const { return m_symbols; }
    const LIndexList& imports() const { return m_imports; }
    const LIndexList& exports() const { return m_exports; }

    ConstIterator lower_bound(RDAddress address, ConstIterator begin) const;
    ConstIterator upper_bound(RDAddress address, ConstIterator begin) const;
    void hex_dump(RDAddress startaddr, RDAddress endaddr);
    void fill(RDAddress startaddr, RDAddress endaddr);
    LIndex type(RDAddress address, RDType t);
    LIndex string(RDAddress address, usize startidx, usize n, char term,
                  RDType t);
    LIndex instruction(RDAddress address);
    LIndex label(RDAddress address);
    LIndex function(RDAddress address);
    LIndex segment(const RDSegment* seg);

public: // State management functions
    tl::optional<usize> field_index() const;
    tl::optional<RDType> current_type() const;
    const RDSegment* current_segment() const { return m_currentsegment; }
    void clear();
    void push_indent(int c = 1);
    void pop_indent(int c = 1);
    void push_fieldindex(usize arg);
    void pop_fieldindex();
    void push_type(RDType t);
    void pop_type();

private:
    LIndex push_item(RDListingItemType type, RDAddress address);
    void check_flags(LIndex lidx, RDAddress address);

private:
    std::deque<usize> m_fieldindex;
    std::deque<RDType> m_currtype;
    const RDSegment* m_currentsegment{nullptr};
    LIndexList m_symbols, m_exports, m_imports;
    usize m_indent{0};
    Type m_items;
};

} // namespace redasm
