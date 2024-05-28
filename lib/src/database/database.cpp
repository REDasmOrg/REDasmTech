#include "database.h"
#include "../error.h"

namespace redasm {

std::string Database::get_name(usize idx, usize ns) const {
    auto it = m_indexdb.find(idx);
    if(it == m_indexdb.end())
        return {};

    return it->second.names[ns];
}

void Database::set_name(usize idx, const std::string& name, usize ns) {
    m_indexdb[idx].names[ns] = name;
}

AddressDetail& Database::get_detail(usize idx) { return m_indexdb[idx]; }

const AddressDetail& Database::get_detail(usize idx) const {
    auto it = m_indexdb.find(idx);

    if(it != m_indexdb.end())
        return it->second;

    except("Cannot find index {}", idx);
}

} // namespace redasm
