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

} // namespace

void register_analyzers() {
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
