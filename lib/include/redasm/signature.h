#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

RD_HANDLE(RDSignatureManager);

typedef struct RDSignatureArgsItem {
    const char* type;
    const char* name;
} RDSignatureArgsItem;

define_slice(RDSignatureArgs, RDSignatureArgsItem);

typedef struct RDSignature {
    const char* name;
    const char* cc;
    const char* ret;
    RDSignatureArgs args;
    HListNode hnode;
} RDSignature;

define_hmap(RDSignatureMap, 8);

REDASM_EXPORT bool rdsignature_add(RDSignatureManager* self, const char* name);
