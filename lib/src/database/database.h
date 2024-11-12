#pragma once

#include "../utils/hash.h"
#include <array>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

namespace redasm {

struct AddressDetail {
    enum NS : u8 {
        LABEL = 0,
        TYPE,
        ARRAY,
        FUNCTION,
        IMPORT,
        EXPORT,
        SEGMENT,

        COUNT,
    };

    struct Ref {
        usize index;
        usize type;
    };

    // AddressDetail(const AddressDetail&) = delete;
    // AddressDetail(AddressDetail&&) = delete;
    // AddressDetail& operator=(const AddressDetail&) = delete;
    // AddressDetail& operator=(AddressDetail&&) = delete;

    std::string type_name;
    std::array<std::string, NS::COUNT> names;
    std::vector<usize> jumps, calls;
    std::vector<Ref> refsto, refs;
    std::string comment;

    union {
        usize string_bytes;
        MIndex flow;
    };
};

struct Database {
private:
    using IndexMap = std::unordered_map<MIndex, AddressDetail>;
    using NamesMap = std::unordered_map<std::string, MIndex, hash::StringHash,
                                        std::equal_to<>>;

public:
    void set_name(MIndex idx, const std::string& name,
                  usize ns = AddressDetail::LABEL);

    [[nodiscard]] std::string get_name(MIndex idx,
                                       usize ns = AddressDetail::LABEL) const;

    [[nodiscard]] tl::optional<usize> get_index(std::string_view name) const;
    AddressDetail& get_detail(MIndex idx);
    const AddressDetail& get_detail(MIndex idx) const;

private:
    IndexMap m_indexdb;
    NamesMap m_names;
};

} // namespace redasm
