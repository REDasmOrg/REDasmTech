#pragma once

#include "database/database.h"
#include "disasm/disassembler.h"
#include "disasm/function.h"
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
#include <string_view>
#include <unordered_set>

namespace redasm {

class Context: public Object {
public:
    static constexpr usize DEFAULT_MIN_STRING = 4;
    static constexpr const char* EP_NAME = "__redasm_ep__";

    Context(const std::shared_ptr<AbstractBuffer>& b, RDLoader* loader);
    bool activate();
    void set_export(MIndex idx);
    void set_import(MIndex idx);
    bool set_function(MIndex idx, const std::string& name = {});
    bool set_entry(MIndex idx, std::string name = EP_NAME);
    void memory_map(RDAddress base, usize size);
    RDAddress memory_copy(MIndex idx, RDOffset start, RDOffset end) const;
    tl::optional<MIndex> address_to_index(RDAddress address) const;
    tl::optional<RDAddress> index_to_address(MIndex idx) const;
    tl::optional<RDOffset> index_to_offset(MIndex idx) const;
    const Segment* index_to_segment(MIndex idx) const;
    const Function* index_to_function(MIndex idx) const;
    bool is_address(RDAddress address) const;

    void map_segment(const std::string& name, MIndex idx, MIndex endidx,
                     RDOffset offset, RDOffset endoffset, usize type);

    bool set_comment(MIndex idx, std::string_view comment = {});

    bool set_type(MIndex idx, std::string_view tname,
                  const std::string& dbname = {});

    bool map_type(MIndex idx, std::string_view tname,
                  const std::string& dbname = {});

    void set_name(MIndex idx, const std::string& name);

    tl::optional<MIndex> get_index(std::string_view name) const;
    std::string get_name(MIndex idx) const;
    std::string get_comment(MIndex idx) const;

    RDAddress memory_copy_n(MIndex idx, RDOffset start, usize size) const {
        return this->memory_copy(idx, start, start + size);
    }

    RDAddress end_baseaddress() const {
        return this->baseaddress + memory->size();
    }

    [[nodiscard]] std::string to_hex(usize v, int n = 0) const;
    void process_functions();
    void process_memory();

private:
    void process_listing_unknown(MIndex& idx);
    void process_listing_data(MIndex& idx);
    void process_listing_code(MIndex& idx);
    void process_listing_array(MIndex& idx, const typing::ParsedType& pt);
    usize process_listing_type(MIndex& idx, const typing::ParsedType& pt);
    void create_function_graph(MIndex idx);

    template<typename Function>
    void process_hex_dump(MIndex& idx, Function f) {
        usize l = 0, start = idx;

        for(; idx < this->memory->size() && f(this->memory->at(idx));
            l++, idx++) {
            if(idx != start && this->memory->at(idx).has(BF_SEGMENT))
                break; // Split inter-segment unknowns

            if(l && !(l % 0x10)) {
                this->listing.hex_dump(start, idx);
                start = idx;
            }
        }

        if(idx > start)
            this->listing.hex_dump(start, idx);
    }

public:
    tl::optional<MIndex> entrypoint;
    tl::optional<int> bits;
    int minstring{DEFAULT_MIN_STRING};
    std::vector<RDAnalyzer> analyzers;
    std::unordered_set<std::string_view> selectedanalyzers;
    std::vector<std::string> availableprocessors;
    std::vector<std::pair<usize, std::string>> collectedtypes;
    std::vector<Segment> segments;
    std::vector<Function> functions;
    Disassembler disassembler;
    RDAddress baseaddress{};
    RDLoader* loader;
    const RDProcessor* processor;
    Listing listing;
    Database database;
    typing::Types types;
    std::unique_ptr<Memory> memory;
    std::shared_ptr<AbstractBuffer> file;

private:
    mutable int m_nchars{0};
};

} // namespace redasm
