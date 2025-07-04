#include "../signature/signature.h"
#include "marshal.h"
#include <redasm/signature.h>
#include <spdlog/spdlog.h>

bool rdsignature_add(RDSignatureManager* self, const char* name) {
    spdlog::trace("rdsignature_add({}, '{}')", fmt::ptr(self), name);
    if(name) return redasm::api::from_c(self)->add(name);
    return false;
}
