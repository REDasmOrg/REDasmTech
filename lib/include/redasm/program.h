#pragma once

#include <redasm/common.h>
#include <redasm/segment.h>
#include <redasm/types.h>

REDASM_EXPORT bool rd_addsegment(const char* name, RDAddress start,
                                 RDAddress end, u32 perm, u32 bits);
REDASM_EXPORT bool rd_addsegment_n(const char* name, RDAddress base, usize n,
                                   u32 perm, u32 bits);
REDASM_EXPORT bool rd_mapfile(RDOffset off, RDAddress start, RDAddress end);
REDASM_EXPORT bool rd_mapfile_n(RDOffset off, RDAddress base, usize n);
