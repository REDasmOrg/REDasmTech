#include "../internal/listing.h"
#include <redasm/listing.h>

bool rdlisting_getindex(RDAddress address, usize* idx) {
    auto lidx = redasm::api::internal::listing_getindex(address);
    if(!lidx)
        return false;

    if(idx)
        *idx = *lidx;

    return true;
}

bool rdlisting_getsymbol(usize idx, RDSymbol* symbol) {
    return redasm::api::internal::listing_getsymbol(idx, symbol);
}

usize rdlisting_getsymbolslength() {
    return redasm::api::internal::listing_getsymbolslength();
}

usize rdlisting_getlength() {
    return redasm::api::internal::listing_getlength();
}
