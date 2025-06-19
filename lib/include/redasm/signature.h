#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

RD_HANDLE(RDSignature);

REDASM_EXPORT bool rdsignature_add(RDSignature* self, const char* name);
