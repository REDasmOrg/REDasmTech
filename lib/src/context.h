#pragma once

#include "database/database.h"
#include "disasm/function.h"
#include "disasm/worker.h"
#include "listing.h"
#include "memory/abstractbuffer.h"
#include "memory/memory.h"
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

    explicit Context(const std::shared_ptr<AbstractBuffer>& b);
    ~Context() override;
    bool try_load(const RDLoaderPlugin* plugin);
    void setup(const RDProcessorPlugin* plugin);
    void set_userdata(const std::string& k, uptr v);
    tl::optional<uptr> get_userdata(const std::string& k) const;
    bool set_function(MIndex idx, usize flags);
    bool set_entry(MIndex idx, const std::string& name = {});
    bool memory_map(RDAddress base, usize size);
    RDAddress memory_copy(MIndex idx, RDOffset start, RDOffset end) const;
    tl::optional<MIndex> address_to_index(RDAddress address) const;
    tl::optional<RDAddress> index_to_address(MIndex idx) const;
    tl::optional<RDOffset> index_to_offset(MIndex idx) const;
    const Segment* index_to_segment(MIndex idx) const;
    const Function* index_to_function(MIndex idx) const;
    bool is_address(RDAddress address) const;

    void map_segment(const std::string& name, MIndex idx, MIndex endidx,
                     RDOffset offset, RDOffset endoffset, usize type);

public: // Database Interface
    void add_ref(MIndex fromidx, MIndex toidx, usize type);
    bool set_comment(MIndex idx, std::string_view comment = {});
    bool set_type(MIndex idx, typing::FullTypeName tname, usize flags);
    bool set_type(MIndex idx, RDType t, usize flags);
    bool set_name(MIndex idx, const std::string& name, usize flags);
    tl::optional<MIndex> get_index(std::string_view name) const;
    tl::optional<RDType> get_type(MIndex idx) const;
    std::string get_name(MIndex idx) const;
    std::string get_comment(MIndex idx) const;
    Database::RefList get_refs_from_type(MIndex fromidx, usize type) const;
    Database::RefList get_refs_from(MIndex fromidx) const;
    Database::RefList get_refs_to_type(MIndex toidx, usize type) const;
    Database::RefList get_refs_to(MIndex toidx) const;

public:
    RDAddress memory_copy_n(MIndex idx, RDOffset start, usize size) const {
        return this->memory_copy(idx, start, start + size);
    }

    RDAddress end_baseaddress() const {
        return this->baseaddress + memory->size();
    }

    void add_problem(MIndex idx, const std::string& s) {
        RDAddress address = this->baseaddress + idx;
        spdlog::warn("{:x}: {}", address, s);
        this->problems.emplace_back(idx, s);
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
    std::unique_ptr<Memory> memory;
    std::shared_ptr<AbstractBuffer> file;
    std::vector<Segment> segments;
    std::vector<std::pair<usize, RDType>> collectedtypes;
    std::vector<std::pair<MIndex, std::string>> problems;
    std::vector<MIndex> entrypoints;
    Worker worker;
    RDAddress baseaddress{};
    FunctionList functions;
    Listing listing;
    typing::Types types;
    int minstring{DEFAULT_MIN_STRING};

private:
    mutable Segment* m_lastsegment{nullptr};
    std::unique_ptr<Database> m_database;
};

} // namespace redasm
