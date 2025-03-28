#pragma once

#include "database/database.h"
#include "disasm/worker.h"
#include "listing.h"
#include "memory/program.h"
#include "typing/typing.h"
#include <redasm/analyzer.h>
#include <redasm/loader.h>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <set>
#include <spdlog/spdlog.h>
#include <string_view>

namespace redasm {

class Context {
    struct AnalyzerSorter {
        bool operator()(const RDAnalyzerPlugin* a,
                        const RDAnalyzerPlugin* b) const {
            return a->order < b->order;
        }
    };

public:
    static constexpr usize DEFAULT_MIN_STRING = 4;

    explicit Context(RDBuffer* file);
    ~Context();
    bool parse(const RDLoaderPlugin* plugin, const RDLoaderRequest* req);
    bool load(const RDProcessorPlugin* plugin);
    void set_userdata(const std::string& k, uptr v);
    tl::optional<uptr> get_userdata(const std::string& k) const;
    bool set_function(RDAddress address, usize flags);
    bool set_entry(RDAddress address, const std::string& name = {});
    void add_problem(RDAddress address, std::string_view s);

public: // Database Interface
    void add_ref(RDAddress fromaddr, RDAddress toaddr, usize type);
    bool set_comment(RDAddress address, std::string_view comment = {});
    bool set_type(RDAddress address, typing::FullTypeName tname, usize flags);
    bool set_type(RDAddress address, RDType t, usize flags);
    bool set_name(RDAddress address, const std::string& name, usize flags);
    tl::optional<RDType> get_type(RDAddress address) const;
    std::string get_name(RDAddress address) const;
    std::string get_comment(RDAddress address) const;
    Database::RefList get_refs_from_type(RDAddress fromaddr, usize type) const;
    Database::RefList get_refs_from(RDAddress fromaddr) const;
    Database::RefList get_refs_to_type(RDAddress fromaddr, usize type) const;
    Database::RefList get_refs_to(RDAddress toaddr) const;
    bool add_sreg_range(RDAddress start, RDAddress end, int sreg, u64 val);
    Database::SRegChanges get_sregs_from_addr(RDAddress address) const;
    Database::SRegChanges get_sreg_changes(int reg) const;
    tl::optional<u64> get_sreg(RDAddress address, int reg) const;
    Database::SRegList get_sregs() const;

    tl::optional<RDAddress> get_address(std::string_view name,
                                        bool onlydb = false) const;

    void set_sreg(RDAddress address, int reg, u64 val,
                  const tl::optional<RDAddress>& fromaddr = tl::nullopt);

public: // Plugins
    const RDLoaderPlugin* loaderplugin{nullptr};
    const RDProcessorPlugin* processorplugin{nullptr};
    RDProcessor* processor{nullptr};
    RDLoader* loader{nullptr};
    std::vector<const RDAnalyzerPlugin*> analyzerplugins;
    std::set<const RDAnalyzerPlugin*, AnalyzerSorter> selectedanalyzerplugins;

public:
    Program program;
    RDProblemSlice problems;
    std::vector<RDAddress> entrypoints;
    Worker* worker{nullptr};
    Listing listing;
    typing::Types types;
    int minstring{DEFAULT_MIN_STRING};

private:
    Database* m_database{nullptr};
};

} // namespace redasm
