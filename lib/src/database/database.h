#pragma once

#include "../utils/hash.h"
#include <array>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <redasm/typing.h>
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
        MIndex index;
        usize type;
    };

    RDType type;
    std::array<std::string, NS::COUNT> names;
    std::vector<MIndex> jumps, calls;
    std::vector<Ref> refsto, refs;
    std::string comment;

    usize string_bytes;
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
    const AddressDetail& get_detail(MIndex idx) const;
    AddressDetail& check_detail(MIndex idx);

private:
    IndexMap m_indexdb;
    NamesMap m_names;
};

} // namespace redasm
