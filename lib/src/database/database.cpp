#include "database.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"
#include "../utils/utils.h"
#include <cctype>

namespace redasm {

std::string Database::get_name(usize idx, usize ns) const {
    auto it = m_indexdb.find(idx);
    if(it == m_indexdb.end())
        return {};

    return it->second.names[ns];
}

void Database::set_name(usize idx, const std::string& name, usize ns) {
    std::string oldname = m_indexdb[idx].names[ns];
    m_indexdb[idx].names[ns] = name;
    m_names[name] = idx;

    if(!oldname.empty())
        m_names.erase(oldname);
}

tl::optional<usize> Database::get_index(const std::string& name) const {
    if(name.empty())
        return tl::nullopt;

    if(auto it = m_names.find(name); it != m_names.end())
        return it->second;

    usize idx = name.size();

    while(idx-- > 0) {
        if(!std::isxdigit(name[idx]))
            break;
    }

    if(++idx < name.size()) {
        std::string_view saddr{name.data() + idx};
        return utils::to_integer<RDAddress>(saddr, 16).and_then(
            [](RDAddress x) { return state::context->address_to_index(x); });
    }

    return tl::nullopt;
}

AddressDetail& Database::get_detail(usize idx) { return m_indexdb[idx]; }

const AddressDetail& Database::get_detail(usize idx) const {
    auto it = m_indexdb.find(idx);

    if(it != m_indexdb.end())
        return it->second;

    except("Cannot find index {}", idx);
}

} // namespace redasm
