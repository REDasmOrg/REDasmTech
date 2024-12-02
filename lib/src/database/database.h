#pragma once

#include "../utils/hash.h"
#include <redasm/processor.h>
#include <redasm/types.h>
#include <redasm/typing.h>
#include <sqlite3.h>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

namespace redasm {

struct AddressDetail {
    RDType type;
    usize string_bytes;
};

struct Database {
private:
    enum class SQLQueries {
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
    };

public:
    struct Ref {
        MIndex index;
        usize type;
    };

    using RefList = std::vector<Ref>;

private:
    using IndexMap = std::unordered_map<MIndex, AddressDetail>;
    using NamesMap = std::unordered_map<std::string, MIndex, hash::StringHash,
                                        std::equal_to<>>;

public:
    explicit Database(std::string_view ldrid, const std::string& source);
    ~Database();

public:
    RefList get_refs_from_type(MIndex fromidx, usize type) const;
    RefList get_refs_from(MIndex fromidx) const;
    RefList get_refs_to_type(MIndex toidx, usize type) const;
    RefList get_refs_to(MIndex toidx) const;
    std::string get_name(MIndex idx) const;
    tl::optional<usize> get_index(std::string_view name,
                                  bool onlydb = false) const;
    std::string get_comment(MIndex idx) const;
    void add_ref(MIndex fromidx, MIndex toidx, usize type);
    void set_comment(MIndex idx, std::string_view comment);
    void set_name(MIndex idx, const std::string& name);

public: // Obsolete
    const AddressDetail& get_detail(MIndex idx) const;
    AddressDetail& check_detail(MIndex idx);

private:
    sqlite3_stmt* prepare_query(SQLQueries q, std::string_view s) const;

private:
    sqlite3* m_db{nullptr};
    mutable std::unordered_map<SQLQueries, sqlite3_stmt*> m_queries;
    std::string m_dbname, m_dbroot;
    IndexMap m_indexdb;
};

} // namespace redasm
