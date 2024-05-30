#include "byte.h"
#include "../marshal.h"

namespace redasm::api::internal {

bool byte_isunknown(RDByte self) { return api::from_c(self).is_unknown(); }
bool byte_iscode(RDByte self) { return api::from_c(self).is_code(); }
bool byte_isdata(RDByte self) { return api::from_c(self).is_data(); }
bool byte_has(RDByte self, usize f) { return api::from_c(self).has(f); }
bool byte_hasbyte(RDByte self) { return api::from_c(self).has_byte(); }

bool byte_getbyte(RDByte self, u8* b) {
    Byte mb = api::from_c(self);

    if(mb.has_byte()) {
        if(b)
            *b = mb.byte();
        return true;
    }

    return false;
}

} // namespace redasm::api::internal
