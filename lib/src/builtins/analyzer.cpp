#include "analyzer.h"
#include "../api/internal/analyzer.h"
#include "../api/internal/memory.h"
#include "../api/internal/redasm.h"
#include "../context.h"
#include "../state.h"
#include <limits>

namespace redasm::builtins {

namespace {

void strings_step(RDAddress& address, const RDMemoryInfo& mi) {
    RDStringResult sr;

    while(address < mi.end) {
        if(api::internal::check_string(address, &sr)) {
            auto index = api::internal::address_to_index(address);

            if(index) {
                Byte b = state::context->memory->at(*index);

                if(b.is_unknown())
                    api::internal::set_type(address, sr.type);
            }

            address += sr.totalsize;
        }
        else
            address++;
    }
}

void do_autorename(const RDAnalyzer*) {
    Context* ctx = state::context;
    const Database& db = ctx->database;

    for(const Segment& seg : ctx->segments) {
        if(!(seg.type & SEGMENTTYPE_HASCODE))
            continue;

        const auto& mem = ctx->memory;

        for(usize i = seg.index; i < seg.endindex; i++) {
            if(Byte b = mem->at(i); !b.has(BF_CALL))
                continue;

            const AddressDetail& d = db.get_detail(i);
            if(d.calls.size() != 1)
                continue;

            if(usize callidx = d.calls.front(); callidx < mem->size()) {
                Byte callb = mem->at(callidx);
                std::string n;

                if(callb.has(BF_JUMP)) {
                    const AddressDetail& jd = db.get_detail(callidx);
                    if(jd.jumps.size() != 1 || jd.jumps.front() >= mem->size())
                        continue;

                    n = "_" + ctx->get_name(jd.jumps.front());
                }
                else if(callb.has(BF_CALL)) {
                    const AddressDetail& cd = db.get_detail(callidx);
                    if(cd.calls.empty() != 1 || cd.calls.front() >= mem->size())
                        continue;

                    n = "_" + ctx->get_name(cd.calls.front());
                }

                if(!n.empty())
                    ctx->set_name(callidx, n);
            }
        }
    }
}

} // namespace

void register_analyzers() {
    RDAnalyzer autorename{};
    autorename.name = "Autorename Nullsubs and Thunks";
    autorename.flags = ANALYZER_SELECTED;
    autorename.order = 0;
    autorename.isenabled = [](const RDAnalyzer*) { return true; };
    autorename.execute = do_autorename; // TODO(davide): Nullsubs
    api::internal::register_analyzer(autorename);

    RDAnalyzer strings{};

    strings.name = "Search and mark all strings";
    strings.order = std::numeric_limits<u32>::max();
    strings.isenabled = [](const RDAnalyzer*) { return true; };

    strings.execute = [](const RDAnalyzer*) {
        RDMemoryInfo mi;
        api::internal::memory_info(&mi);

        RDAddress address = mi.start;

        while(address < mi.end)
            strings_step(address, mi);
    };

    api::internal::register_analyzer(strings);
}

} // namespace redasm::builtins
