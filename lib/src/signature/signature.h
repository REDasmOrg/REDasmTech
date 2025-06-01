#pragma once

#include <string>

namespace redasm::signature {

struct Signature {
    std::string name;
};

const Signature* find(const std::string& name);

} // namespace redasm::signature
