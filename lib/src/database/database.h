#pragma once

#include <redasm/redasm.h>
#include <redasm/typing.h>
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm {

struct Database {
public:
    using RefList = std::vector<RDRef>;

public:
    explicit Database(std::string_view ldrid, std::string_view source);
    ~Database();

public:
    RefList get_refs_from_type(RDAddress fromaddr, usize type) const;
    RefList get_refs_from(RDAddress fromaddr) const;
    RefList get_refs_to_type(RDAddress toaddr, usize type) const;
    RefList get_refs_to(RDAddress toaddr) const;
    std::string get_name(RDAddress address) const;
    std::string get_comment(RDAddress address) const;
    tl::optional<RDType> get_type(RDAddress address) const;
    void add_ref(RDAddress fromaddr, RDAddress toaddr, usize type);
    void set_comment(RDAddress address, std::string_view comment);
    void set_name(RDAddress address, std::string_view name);
    void set_type(RDAddress address, RDType t);
    void set_userdata(std::string_view k, uptr v);
    tl::optional<uptr> get_userdata(std::string_view k) const;

    void add_segment(std::string_view name, RDAddress startaddr,
                     RDAddress endaddr, u32 perm, u32 bits);

    tl::optional<RDAddress> get_address(std::string_view name,
                                        bool onlydb = false) const;

private:
    sqlite3_stmt* prepare_query(int q, std::string_view s) const;

private:
    sqlite3* m_db{nullptr};
    mutable std::unordered_map<int, sqlite3_stmt*> m_queries;
    std::string m_dbname, m_dbroot;
};

} // namespace redasm
