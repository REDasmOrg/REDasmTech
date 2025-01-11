#pragma once

#include "basetypes.h"
#include "value.h"
#include <forward_list>

namespace redasm::typing {

struct Types: public BaseTypes {
    const TypeDef* declare(const std::string& name, const Struct& arg);
    std::forward_list<Value> valuespool;
};

} // namespace redasm::typing
