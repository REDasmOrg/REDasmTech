#pragma once

#include <Zydis/Zydis.h>
#include <optional>
#include <redasm/redasm.h>

namespace x86_common {

[[nodiscard]] std::optional<RDAddress> read_address(RDAddress address);

ZydisRegister get_ip();
ZydisRegister get_sp();
ZydisRegister get_bp();

} // namespace x86_common
