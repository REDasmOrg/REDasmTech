#include "../signature/signature.h"
#include "marshal.h"
#include <redasm/signature.h>
#include <spdlog/spdlog.h>

bool rdsignature_add(RDSignature* self, const char* name) {
    spdlog::trace("rdsignature_add({}, '{}')", fmt::ptr(self), name);
    if(name) return redasm::api::from_c(self)->load(name);
    return false;
}
