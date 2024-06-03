#pragma once

#include <array>
#include <redasm/processor.h>
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

    // AddressDetail(const AddressDetail&) = delete;
    // AddressDetail(AddressDetail&&) = delete;
    // AddressDetail& operator=(const AddressDetail&) = delete;
    // AddressDetail& operator=(AddressDetail&&) = delete;

    std::string type_name;
    std::array<std::string, NS::COUNT> names;
    std::vector<usize> jumps, calls;
    std::vector<RDRef> refsto, refs;
    std::vector<std::string> comments;

    union {
        usize string_length;
        usize flow;
    };
};

struct Database {
    void set_name(usize idx, const std::string& name,
                  usize ns = AddressDetail::LABEL);

    [[nodiscard]] std::string get_name(usize idx,
                                       usize ns = AddressDetail::LABEL) const;

    [[nodiscard]] tl::optional<usize> get_index(const std::string& name) const;
    AddressDetail& get_detail(usize idx);
    const AddressDetail& get_detail(usize idx) const;

private:
    std::unordered_map<usize, AddressDetail> m_indexdb;
    std::unordered_map<std::string, usize> m_names;
};

} // namespace redasm
