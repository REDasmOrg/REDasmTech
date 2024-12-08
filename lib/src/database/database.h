#pragma once

#include <redasm/typing.h>
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm {

struct Database {
public:
    struct Ref {
        MIndex index;
        usize type;
    };

    using RefList = std::vector<Ref>;

public:
    explicit Database(std::string_view ldrid, std::string_view source);
    ~Database();

public:
    RefList get_refs_from_type(MIndex fromidx, usize type) const;
    RefList get_refs_from(MIndex fromidx) const;
    RefList get_refs_to_type(MIndex toidx, usize type) const;
    RefList get_refs_to(MIndex toidx) const;
    std::string get_name(MIndex idx) const;
    std::string get_comment(MIndex idx) const;
    tl::optional<RDType> get_type(MIndex idx) const;
    void add_ref(MIndex fromidx, MIndex toidx, usize type);
    void set_comment(MIndex idx, std::string_view comment);
    void set_name(MIndex idx, std::string_view name);
    void set_type(MIndex idx, RDType t);

    void add_segment(std::string_view name, MIndex idx, MIndex endidx,
                     RDOffset offset, RDOffset endoffset, usize type);

    tl::optional<usize> get_index(std::string_view name,
                                  bool onlydb = false) const;

private:
    sqlite3_stmt* prepare_query(int q, std::string_view s) const;

private:
    sqlite3* m_db{nullptr};
    mutable std::unordered_map<int, sqlite3_stmt*> m_queries;
    std::string m_dbname, m_dbroot;
};

} // namespace redasm
