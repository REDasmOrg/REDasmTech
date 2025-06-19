#pragma once

#include <redasm/signature.h>
#include <redasm/typing.h>
#include <string>
#include <string_view>
#include <unordered_map>

namespace redasm::signature {

struct FunctionSignature {
    std::string calling_convention;
    RDType ret;
};

struct PatternSignature {
    int reserved;
};

struct Signature {
    int type;

    union {
        FunctionSignature func;
        PatternSignature pattern;
    };
};

struct SignatureManager {
    bool load(std::string_view name);
};

void compile();

} // namespace redasm::signature
