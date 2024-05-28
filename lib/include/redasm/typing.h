#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef struct RDStructField {
    const char* type;
    const char* name;
} RDStructField;

REDASM_EXPORT usize rd_sizeof(const char* tname);
REDASM_EXPORT const char* rd_createstruct(const char* name,
                                          const RDStructField* fields, usize n);
