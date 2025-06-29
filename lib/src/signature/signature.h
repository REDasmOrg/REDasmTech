#pragma once

#include <redasm/signature.h>
#include <redasm/typing.h>
#include <string_view>

namespace redasm::signature {

struct SignatureManager {
    SignatureManager();
    ~SignatureManager();
    bool add(std::string_view sigfile);
    [[nodiscard]] bool empty() const;
    [[nodiscard]] const RDSignature* find(std::string_view n) const;

    RDSignatureMap signatures;
};

} // namespace redasm::signature
