#include "database.h"
#include <cctype>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace redasm {

namespace fs = std::filesystem;

struct SQLQueries {
    enum {
        SET_NAME = 0,
        GET_NAME,
        GET_INDEX,
        ADD_REF,
        GET_REFS_FROM_TYPE,
        GET_REFS_FROM,
        GET_REFS_TO_TYPE,
        GET_REFS_TO,
        SET_COMMENT,
        GET_COMMENT,
        SET_TYPE,
        GET_TYPE,
        ADD_SEGMENT,
        SET_SREG,
        GET_SREG,
        GET_SREGS_FROM_ADDR,
        GET_SREG_CHANGES,
        GET_SREGS,
        SET_USERDATA,
        GET_USERDATA,
    };
};

namespace {

constexpr std::string_view DATABASE_FILE = "database.sqlite";

// clang-format off
template<typename T>
concept SQLBindable =
    std::same_as<T, std::nullptr_t> || 
    std::same_as<T, std::string> || 
    std::same_as<T, std::string_view> ||
    std::same_as<T, RDAddress> || 
    std::same_as<T, isize> ||
    std::same_as<T, const char*> ||
    std::same_as<T, int> ||
    std::same_as<T, u64> ||
    std::same_as<T, u32> ||
    std::same_as<T, u8>;
// clang-format on

template<SQLBindable T>
void sql_bindparam(sqlite3* db, sqlite3_stmt* stmt, std::string_view n,
                   const T& v) {
    using U = std::decay_t<T>;

    int idx = sqlite3_bind_parameter_index(stmt, n.data());
    if(!idx) {
        ct_exceptf("SQL: parameter '%.*s' not found",
                   static_cast<int>(n.size()), n.data());
    }

    int res = SQLITE_ERROR;
    if constexpr(std::is_same_v<U, std::string> ||
                 std::is_same_v<U, std::string_view>)
        res = sqlite3_bind_text(stmt, idx, v.data(), v.size(), SQLITE_STATIC);
    else if constexpr(std::is_same_v<U, RDAddress> || std::is_same_v<U, u64> ||
                      std::is_same_v<U, isize>)
        res = sqlite3_bind_int64(stmt, idx, static_cast<sqlite3_int64>(v));
    else if constexpr(std::is_same_v<U, u32> || std::is_same_v<U, u8> ||
                      std::is_same_v<U, int>)
        res = sqlite3_bind_int(stmt, idx, static_cast<int>(v));
    else if constexpr(std::is_same_v<U, std::nullptr_t>)
        res = sqlite3_bind_null(stmt, idx);
    else if constexpr(std::is_same_v<U, const char*>) {
        std::string_view sv = v;
        res = sqlite3_bind_text(stmt, idx, sv.data(), sv.size(), SQLITE_STATIC);
    }

    if(res != SQLITE_OK) ct_exceptf("SQL: %s", sqlite3_errmsg(db));
}

int sql_step(sqlite3* db, sqlite3_stmt* stmt) {
    int res = sqlite3_step(stmt);
    if((res == SQLITE_ROW) || (res == SQLITE_DONE)) return res;
    ct_exceptf("SQL: %s", sqlite3_errmsg(db));
}

constexpr std::string_view DB_SCHEMA = R"(
    PRAGMA synchronous = OFF;
    PRAGMA journal_mode = MEMORY;

    CREATE TABLE Info(
        k TEXT PRIMARY KEY,
        v TEXT NOT NULL
    );

    CREATE TABLE UserData(
        k TEXT PRIMARY KEY,
        v INTEGER NOT NULL
    );

    CREATE TABLE Segments(
        name TEXT NOT NULL,
        startaddr INTEGER NOT NULL,
        endaddr INTEGER NOT NULL,
        perm INTEGER NOT NULL,
        bits INTEGER NOT NULL
    );

    CREATE TABLE Comments(
        address INTEGER PRIMARY KEY,
        comment TEXT NOT NULL
    );

    CREATE TABLE Refs(
        fromaddr INTEGER NOT NULL,
        toaddr INTEGER NOT NULL,
        type INTEGER NOT NULL,
        UNIQUE(fromaddr, toaddr)
    );

    CREATE TABLE Types(
        address INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        n INTEGER NOT NULL
    );

    CREATE TABLE Names(
        address INTEGER PRIMARY KEY,
        name TEXT NOT NULL
    );

    CREATE TABLE SegmentRegisters(
        address INTEGER NOT NULL,
        reg INTEGER NOT NULL,
        value INTEGER,
        fromaddr INTEGER,
        UNIQUE(address, reg, fromaddr)
    )
)";

} // namespace

Database::Database(std::string_view ldrid, std::string_view source) {
    ct_assume(!source.empty());

    m_dbname = fs::path{source}.filename().replace_extension(".rdb").string();
    m_dbroot = (fs::path{source}.remove_filename() / m_dbname / ldrid).string();
    ct_assume(!m_dbroot.empty());

    if(fs::exists(m_dbroot)) // Remove old database
        fs::remove_all(m_dbroot);

    spdlog::info("Loading database: {}", m_dbroot);
    fs::create_directories(m_dbroot);

    fs::path dbfile = fs::path{m_dbroot} / DATABASE_FILE;
    ct_assume(!sqlite3_open(dbfile.string().c_str(), &m_db));

    char* errmsg = nullptr;
    int rc = sqlite3_exec(m_db, DB_SCHEMA.data(), nullptr, nullptr, &errmsg);

    if(rc != SQLITE_OK) ct_exceptf("SQLite Error: %s", errmsg);
}

Database::~Database() {
    if(m_db) {
        for(const auto& [_, stmt] : m_queries)
            sqlite3_finalize(stmt);

        sqlite3_close(m_db);
        m_db = nullptr;
    }

    if(fs::exists(m_dbroot)) {
        spdlog::info("Unloading database: {}", m_dbroot);
        fs::remove_all(m_dbroot);
    }

    // Check project root, delete it if empty
    fs::path projroot = fs::path{m_dbroot}.parent_path();

    if(fs::exists(projroot) && fs::is_empty(projroot)) fs::remove_all(projroot);
}

sqlite3_stmt* Database::prepare_query(int q, std::string_view s) const {
    sqlite3_stmt* stmt = nullptr;

    if(auto it = m_queries.find(q); it == m_queries.end()) {
        if(int rc =
               sqlite3_prepare_v2(m_db, s.data(), s.size(), &stmt, nullptr);
           rc != SQLITE_OK) {
            ct_exceptf("SQL: %s\nQUERY: %.*s", sqlite3_errmsg(m_db),
                       static_cast<int>(s.size()), s.data());
        }

        m_queries[q] = stmt;
    }
    else {
        stmt = m_queries[q];
        sqlite3_reset(stmt);
    }

    return stmt;
}

void Database::add_segment(std::string_view name, RDAddress startaddr,
                           RDAddress endaddr, u32 perm, u32 bits) {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::ADD_SEGMENT, R"(
        INSERT INTO Segments
            VALUES (:name, :startaddr, :endaddr, :perm, :bits)
    )");

    sql_bindparam(m_db, stmt, ":name", name);
    sql_bindparam(m_db, stmt, ":startaddr", startaddr);
    sql_bindparam(m_db, stmt, ":endaddr", endaddr);
    sql_bindparam(m_db, stmt, ":perm", perm);
    sql_bindparam(m_db, stmt, ":bits", bits);
    sql_step(m_db, stmt);
}

std::string Database::get_name(RDAddress address) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_NAME, R"(
        SELECT name 
        FROM Names 
        WHERE address = :address
    )");

    sql_bindparam(m_db, stmt, ":address", address);

    if(sql_step(m_db, stmt) == SQLITE_ROW)
        return reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    return {};
}

void Database::add_ref(RDAddress fromaddr, RDAddress toaddr, usize type) {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::ADD_REF, R"(
        INSERT INTO Refs
            VALUES (:fromaddr, :toaddr, :type)
        ON CONFLICT DO 
            UPDATE SET type = EXCLUDED.type
    )");

    sql_bindparam(m_db, stmt, ":fromaddr", fromaddr);
    sql_bindparam(m_db, stmt, ":toaddr", toaddr);
    sql_bindparam(m_db, stmt, ":type", type);
    sql_step(m_db, stmt);
}

tl::optional<u64> Database::get_sreg(RDAddress addr, int reg) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_SREG, R"(
        SELECT value
        FROM SegmentRegisters
        WHERE address = :address AND reg == :reg
    )");

    sql_bindparam(m_db, stmt, ":address", addr);
    sql_bindparam(m_db, stmt, ":reg", reg);

    if(sql_step(m_db, stmt) == SQLITE_ROW)
        return static_cast<u64>(sqlite3_column_int64(stmt, 0));

    return tl::nullopt;
}

Database::SRegChanges Database::get_sregs_from_addr(RDAddress addr) const {
    sqlite3_stmt* stmt =
        this->prepare_query(SQLQueries::GET_SREGS_FROM_ADDR, R"(
        SELECT reg, value, fromaddr
        FROM SegmentRegisters
        WHERE address = :address
    )");

    sql_bindparam(m_db, stmt, ":address", addr);

    SRegChanges rc;

    while(sql_step(m_db, stmt) == SQLITE_ROW) {
        tl::optional<RDAddress> fromaddr = tl::nullopt;
        if(sqlite3_column_type(stmt, 2) != SQLITE_NULL)
            fromaddr = static_cast<RDAddress>(sqlite3_column_int64(stmt, 2));

        rc.emplace_back(addr, sqlite3_column_int(stmt, 0),
                        static_cast<u64>(sqlite3_column_int64(stmt, 1)),
                        fromaddr);
    }

    return rc;
}

Database::SRegChanges Database::get_sreg_changes(int sreg) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_SREG_CHANGES, R"(
        SELECT address, value, fromaddr
        FROM SegmentRegisters
        WHERE reg = :reg
    )");

    sql_bindparam(m_db, stmt, ":reg", sreg);

    SRegChanges rc;

    while(sql_step(m_db, stmt) == SQLITE_ROW) {
        tl::optional<RDAddress> fromaddr = tl::nullopt;
        if(sqlite3_column_type(stmt, 2) != SQLITE_NULL)
            fromaddr = static_cast<RDAddress>(sqlite3_column_int64(stmt, 2));

        rc.emplace_back(static_cast<u64>(sqlite3_column_int64(stmt, 0)), sreg,
                        static_cast<u64>(sqlite3_column_int64(stmt, 1)),
                        fromaddr);
    }

    return rc;
}

Database::SRegList Database::get_sregs() const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_SREGS, R"(
        SELECT DISTINCT reg
        FROM SegmentRegisters
    )");

    Database::SRegList sregs;

    while(sql_step(m_db, stmt) == SQLITE_ROW)
        sregs.emplace_back(sqlite3_column_int(stmt, 0));

    return sregs;
}

void Database::set_sreg(RDAddress addr, int reg, const RDRegValue& val,
                        const tl::optional<RDAddress>& fromaddr) {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::SET_SREG, R"(
        INSERT INTO SegmentRegisters (address, reg, value, fromaddr) 
            VALUES (:address, :reg, :val, :fromaddr)
        ON CONFLICT DO 
            UPDATE SET reg = EXCLUDED.reg
    )");

    sql_bindparam(m_db, stmt, ":address", addr);
    sql_bindparam(m_db, stmt, ":reg", reg);

    if(val.ok)
        sql_bindparam(m_db, stmt, ":val", val.value);
    else
        sql_bindparam(m_db, stmt, ":val", nullptr);

    if(fromaddr.has_value())
        sql_bindparam(m_db, stmt, ":fromaddr", *fromaddr);
    else
        sql_bindparam(m_db, stmt, ":fromaddr", nullptr);

    sql_step(m_db, stmt);
}

void Database::set_comment(RDAddress address, std::string_view comment) {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::SET_COMMENT, R"(
        INSERT INTO Comments (address, comment) 
            VALUES (:address, :comment)
        ON CONFLICT DO 
            UPDATE SET comment = EXCLUDED.comment
    )");

    sql_bindparam(m_db, stmt, ":address", address);
    sql_bindparam(m_db, stmt, ":comment", comment);
    sql_step(m_db, stmt);
}

Database::RefList Database::get_refs_from_type(RDAddress fromaddr,
                                               usize type) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_REFS_FROM_TYPE, R"(
        SELECT toaddr, type 
        FROM Refs
        WHERE fromaddr = :fromaddr AND type = :type
    )");

    sql_bindparam(m_db, stmt, ":fromaddr", fromaddr);
    sql_bindparam(m_db, stmt, ":type", type);

    RefList res;

    while(sql_step(m_db, stmt) == SQLITE_ROW) {
        res.emplace_back(static_cast<RDAddress>(sqlite3_column_int64(stmt, 0)),
                         static_cast<usize>(sqlite3_column_int64(stmt, 1)));
    }

    return res;
}

Database::RefList Database::get_refs_from(RDAddress fromaddr) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_REFS_FROM, R"(
        SELECT toaddr, type
        FROM Refs
        WHERE fromaddr = :fromaddr
    )");

    sql_bindparam(m_db, stmt, ":fromaddr", fromaddr);

    RefList rl;

    while(sql_step(m_db, stmt) == SQLITE_ROW) {
        rl.emplace_back(static_cast<RDAddress>(sqlite3_column_int64(stmt, 0)),
                        static_cast<usize>(sqlite3_column_int64(stmt, 1)));
    }

    return rl;
}

Database::RefList Database::get_refs_to_type(RDAddress toaddr,
                                             usize type) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_REFS_TO_TYPE, R"(
        SELECT fromaddr, type
        FROM Refs
        WHERE toaddr = :toaddr AND type = :type
    )");

    sql_bindparam(m_db, stmt, ":toaddr", toaddr);
    sql_bindparam(m_db, stmt, ":type", type);

    RefList res;

    while(sql_step(m_db, stmt) == SQLITE_ROW) {
        res.emplace_back(static_cast<RDAddress>(sqlite3_column_int64(stmt, 0)),
                         static_cast<usize>(sqlite3_column_int64(stmt, 1)));
    }

    return res;
}

Database::RefList Database::get_refs_to(RDAddress toaddr) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_REFS_TO, R"(
        SELECT fromaddr, type 
        FROM Refs
        WHERE toaddr = :toaddr
    )");

    sql_bindparam(m_db, stmt, ":toaddr", toaddr);

    RefList res;

    while(sql_step(m_db, stmt) == SQLITE_ROW) {
        res.emplace_back(static_cast<RDAddress>(sqlite3_column_int64(stmt, 0)),
                         static_cast<usize>(sqlite3_column_int64(stmt, 1)));
    }

    return res;
}

void Database::set_name(RDAddress address, std::string_view name) {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::SET_NAME, R"(
        INSERT INTO Names
            VALUES (:address, :name)
        ON CONFLICT DO 
            UPDATE SET name = EXCLUDED.name
    )");

    sql_bindparam(m_db, stmt, ":address", address);
    sql_bindparam(m_db, stmt, ":name", name);
    sql_step(m_db, stmt);
}

void Database::set_type(RDAddress address, RDType t) {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::SET_TYPE, R"(
        INSERT INTO Types
            VALUES (:address, :name, :n)
        ON CONFLICT DO 
            UPDATE SET name = EXCLUDED.name, n = EXCLUDED.n
    )");

    ct_assume(t.def);

    sql_bindparam(m_db, stmt, ":address", address);
    sql_bindparam(m_db, stmt, ":name", t.def->name);
    sql_bindparam(m_db, stmt, ":n", t.n);
    sql_step(m_db, stmt);
}

void Database::set_userdata(std::string_view k, uptr v) {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::SET_USERDATA, R"(
        INSERT INTO UserData
            VALUES (:k, :v)
        ON CONFLICT DO 
            UPDATE SET v = EXCLUDED.v
    )");

    sql_bindparam(m_db, stmt, ":k", k);
    sql_bindparam(m_db, stmt, ":v", v);
    sql_step(m_db, stmt);
}

tl::optional<uptr> Database::get_userdata(std::string_view k) const {
    if(k.empty()) return tl::nullopt;

    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_USERDATA, R"(
        SELECT v 
        FROM UserData
        WHERE k = :k
    )");

    sql_bindparam(m_db, stmt, ":k", k);

    if(sql_step(m_db, stmt) == SQLITE_ROW)
        return static_cast<uptr>(sqlite3_column_int64(stmt, 0));

    return tl::nullopt;
}

tl::optional<RDAddress> Database::get_address(std::string_view name) const {
    if(name.empty()) return tl::nullopt;

    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_INDEX, R"(
        SELECT address 
        FROM Names 
        WHERE name = :name
    )");

    sql_bindparam(m_db, stmt, ":name", name);

    if(sql_step(m_db, stmt) == SQLITE_ROW)
        return static_cast<RDAddress>(sqlite3_column_int64(stmt, 0));

    return tl::nullopt;
}

std::string Database::get_comment(RDAddress address) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_COMMENT, R"(
        SELECT comment 
        FROM Comments 
        WHERE address = :address
    )");

    sql_bindparam(m_db, stmt, ":address", address);

    if(sql_step(m_db, stmt) == SQLITE_ROW)
        return reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    return {};
}

tl::optional<Database::Type> Database::get_type(RDAddress address) const {
    sqlite3_stmt* stmt = this->prepare_query(SQLQueries::GET_COMMENT, R"(
        SELECT name,n
        FROM Types
        WHERE address = :address
    )");

    sql_bindparam(m_db, stmt, ":address", address);

    if(sql_step(m_db, stmt) == SQLITE_ROW) {
        return Database::Type{
            .name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            .n = static_cast<u64>(sqlite3_column_int64(stmt, 1)),
        };
    }

    return tl::nullopt;
}

} // namespace redasm
