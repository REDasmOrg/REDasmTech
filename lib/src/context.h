#pragma once

#include "database/database.h"
#include "disasm/function.h"
#include "disasm/worker.h"
#include "listing.h"
#include "memory/program.h"
#include "segment.h"
#include "typing/typing.h"
#include "utils/object.h"
#include <memory>
#include <redasm/analyzer.h>
#include <redasm/loader.h>
#include <redasm/processor.h>
#include <redasm/types.h>
#include <set>
#include <string_view>

namespace redasm {

class Context: public Object {
    struct AnalyzerSorter {
        bool operator()(const RDAnalyzerPlugin* a,
                        const RDAnalyzerPlugin* b) const {
            return a->order < b->order;
        }
    };

public:
    static constexpr usize DEFAULT_MIN_STRING = 4;

    explicit Context(RDBuffer* b);
    ~Context() override;
    bool try_load(const RDLoaderPlugin* plugin);
    void setup(const RDProcessorPlugin* plugin);
    void set_userdata(const std::string& k, uptr v);
    tl::optional<uptr> get_userdata(const std::string& k) const;
    bool set_function(RDAddress address, usize flags);
    bool set_entry(RDAddress address, const std::string& name = {});
    bool memory_map(RDAddress base, usize size);
    tl::optional<MIndex> address_to_index(RDAddress address) const;
    const Function* find_function(RDAddress address) const;

public: // Database Interface
    void add_ref(RDAddress fromaddr, RDAddress toaddr, usize type);
    bool set_comment(RDAddress address, std::string_view comment = {});
    bool set_type(RDAddress address, typing::FullTypeName tname, usize flags);
    bool set_type(RDAddress address, RDType t, usize flags);
    bool set_name(RDAddress address, const std::string& name, usize flags);
    tl::optional<RDAddress> get_address(std::string_view name) const;
    tl::optional<RDType> get_type(RDAddress address) const;
    std::string get_name(RDAddress address) const;
    std::string get_comment(RDAddress address) const;
    Database::RefList get_refs_from_type(RDAddress fromaddr, usize type) const;
    Database::RefList get_refs_from(RDAddress fromaddr) const;
    Database::RefList get_refs_to_type(RDAddress fromaddr, usize type) const;
    Database::RefList get_refs_to(RDAddress toaddr) const;

public:
    void add_problem(RDAddress address, const std::string& s) {
        spdlog::warn("add_problem(): {:x} = {}", address, s);
        this->problems.emplace_back(address, s);
    }

    [[nodiscard]] std::string to_hex(usize v, int n = -1) const;

public: // Plugins
    const RDLoaderPlugin* loaderplugin{nullptr};
    const RDProcessorPlugin* processorplugin{nullptr};
    RDProcessor* processor{nullptr};
    RDLoader* loader{nullptr};
    std::vector<const RDAnalyzerPlugin*> analyzerplugins;
    std::set<const RDAnalyzerPlugin*, AnalyzerSorter> selectedanalyzerplugins;

public:
    Program program;
    std::vector<std::pair<usize, RDType>> collectedtypes;
    std::vector<std::pair<RDAddress, std::string>> problems;
    std::vector<RDAddress> entrypoints;
    Worker worker;
    FunctionList functions;
    Listing listing;
    typing::Types types;
    int minstring{DEFAULT_MIN_STRING};

private:
    std::unique_ptr<Database> m_database;
};

} // namespace redasm
