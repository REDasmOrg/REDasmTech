#pragma once

#include "basetypes.h"

namespace redasm::typing {

struct Types: public BaseTypes {
    const TypeDef* declare(const std::string& name,
                           const RDStructField* fields);
};

} // namespace redasm::typing
