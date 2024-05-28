#pragma once

#include <array>
#include <redasm/types.h>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

namespace redasm {

struct AddressDetail {
    enum NS : usize {
        LABEL = 0,
        TYPE,
        ARRAY,
        FUNCTION,
        IMPORT,
        EXPORT,
        SEGMENT,

        COUNT,
    };

    enum REF : usize {
        REF_DATA,
        REF_CALL,
        REF_BRANCH,
    };

    // AddressDetail(const AddressDetail&) = delete;
    // AddressDetail(AddressDetail&&) = delete;
    // AddressDetail& operator=(const AddressDetail&) = delete;
    // AddressDetail& operator=(AddressDetail&&) = delete;

    std::string type_name;
    std::array<std::string, NS::COUNT> names;
    std::vector<usize> refs, next;

    union {
        usize segment_index;
        usize string_length;
    };
};

struct Database {
    void set_name(usize idx, const std::string& name,
                  usize ns = AddressDetail::LABEL);

    std::string get_name(usize idx, usize ns = AddressDetail::LABEL) const;
    AddressDetail& get_detail(usize idx);
    const AddressDetail& get_detail(usize idx) const;

private:
    std::unordered_map<usize, AddressDetail> m_indexdb;
};

} // namespace redasm
