#include "byte.h"

namespace redasm::api::internal {

bool byte_isunknown(Byte self) { return self.is_unknown(); }
bool byte_iscode(Byte self) { return self.is_code(); }
bool byte_isdata(Byte self) { return self.is_data(); }
bool byte_has(Byte self, RDByteFlags f) { return self.has(f); }
bool byte_hasbyte(Byte self) { return self.has_byte(); }

bool byte_getbyte(Byte self, u8* b) {
    if(self.has_byte()) {
        if(b)
            *b = self.byte();
        return true;
    }

    return false;
}

} // namespace redasm::api::internal
