#include "database.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"
#include "../utils/utils.h"
#include <cctype>
#include <filesystem>

namespace redasm {

namespace fs = std::filesystem;

namespace {

constexpr std::string_view DB_SCHEMA = R"(
CREATE TABLE MetaData(
    k TEXT NOT NULL,
    v TEXT NOT NULL
);

CREATE TABLE Comments(
    idx INT PRIMARY KEY,
    value TEXT
);

CREATE TABLE Refs(
    fromidx INT NOT NULL,
    toidx INT NOT NULL,
    type INT NOT NULL
);

CREATE TABLE Names(
    idx INT PRIMARY KEY,
    label TEXT,
    array TEXT,
    func TEXT,
    import TEXT,
    export TEXT,
    segment TEXT
);
)";

}

Database::Database(const std::string& source) {
    assume(!source.empty());

    std::string dbname = fs::path{source}.stem().replace_extension(".sqlite");

    // We don't have project support atm: delete old db
    m_dbpath = fs::path{source}.remove_filename() / dbname;
    if(fs::exists(m_dbpath))
        fs::remove(m_dbpath);

    spdlog::critical(">>>> {}", m_dbpath);
    assume(!sqlite3_open(m_dbpath.c_str(), &m_db));

    char* errmsg = nullptr;
    int rc = sqlite3_exec(m_db, DB_SCHEMA.data(), nullptr, nullptr, &errmsg);

    if(rc != SQLITE_OK)
        except("SQLite Error: {}", errmsg);
}

Database::~Database() {
    if(m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }

    if(!m_dbpath.empty() && fs::exists(m_dbpath))
        fs::remove(m_dbpath);
}

std::string Database::get_name(MIndex idx, usize ns) const {
    auto it = m_indexdb.find(idx);
    if(it == m_indexdb.end())
        return {};

    return it->second.names[ns];
}

void Database::set_name(MIndex idx, const std::string& name, usize ns) {
    std::string oldname = m_indexdb[idx].names[ns];
    m_indexdb[idx].names[ns] = name;
    m_names[name] = idx;

    if(!oldname.empty())
        m_names.erase(oldname);
}

tl::optional<MIndex> Database::get_index(std::string_view name) const {
    if(name.empty())
        return tl::nullopt;

    if(auto it = m_names.find(name); it != m_names.end())
        return it->second;

    usize idx = name.size();

    while(idx-- > 0) {
        if(!std::isxdigit(name[idx]))
            break;
    }

    if(idx >= name.size() || name.at(idx) != '_')
        return tl::nullopt;

    if(++idx < name.size()) {
        std::string_view saddr{name.data() + idx};
        return utils::to_integer<RDAddress>(saddr, 16).and_then(
            [](RDAddress x) { return state::context->address_to_index(x); });
    }

    return tl::nullopt;
}

AddressDetail& Database::check_detail(MIndex idx) { return m_indexdb[idx]; }

const AddressDetail& Database::get_detail(MIndex idx) const {
    auto it = m_indexdb.find(idx);

    if(it != m_indexdb.end())
        return it->second;

    except("Cannot find index {:x}", idx);
}

} // namespace redasm
