#pragma once

#include "database/database.h"
#include "disasm/disassembler.h"
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

    Context(const std::shared_ptr<AbstractBuffer>& b, const RDLoader* loader);
    void set_export(usize idx, const std::string& name);
    void set_import(usize idx, const std::string& name);
    void memory_map(RDAddress base, usize size);
    RDAddress memory_copy(usize idx, RDOffset start, RDOffset end) const;
    tl::optional<usize> address_to_index(RDAddress address) const;
    tl::optional<RDAddress> index_to_address(usize index) const;
    tl::optional<RDOffset> index_to_offset(usize index) const;
    const Segment* index_to_segment(usize index) const;
    bool is_address(RDAddress address) const;

    void map_segment(const std::string& name, usize idx, usize endidx,
                     RDOffset offset, RDOffset endoffset, usize type);

    bool set_type(usize idx, std::string_view tname,
                  const std::string& dbname = {});

    bool map_type(usize idx, std::string_view tname,
                  const std::string& dbname = {});

    void set_name(usize idx, const std::string& name);

    std::string get_name(usize idx) const;

    inline RDAddress memory_copy_n(usize idx, RDOffset start,
                                   usize size) const {
        return this->memory_copy(idx, start, start + size);
    }

    inline RDAddress end_baseaddress() const {
        return this->baseaddress + memory->size();
    }

    [[nodiscard]] std::string to_hex(usize v, int n = 0) const;
    void build_listing();

private:
    void process_listing_unknown(usize& idx);
    void process_listing_data(usize& idx);
    void process_listing_code(usize& idx);
    void process_listing_array(usize& idx, const typing::ParsedType& pt);
    usize process_listing_type(usize& idx, const typing::ParsedType& pt);

public:
    int bits{}, minstring{DEFAULT_MIN_STRING};
    std::unordered_set<const RDAnalyzer*> selectedanalyzers;
    std::vector<std::string> availableprocessors;
    std::vector<std::pair<usize, std::string>> collectedtypes;
    std::vector<Segment> segments;
    Disassembler disassembler;
    RDAddress baseaddress{};
    const RDLoader* loader;
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
