/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Functions to parse and manipulate the additional data tables added
 * to optimized .dex files.
 */

#include "DexOptData.h"
#include <redasm/hash.h>

/*
 * Check to see if a given data pointer is a valid double-word-aligned
 * pointer into the given memory range (from start inclusive to end
 * exclusive). Returns true if valid.
 */
static bool isValidPointer(const void* ptr, const void* start,
                           const void* end) {
    return (ptr >= start) && (ptr < end) && (((uintptr_t)ptr & 7) == 0);
}

/* (documented in header file) */
dex_u4 dexComputeOptChecksum(const DexOptHeader* pOptHeader) {
    const dex_u1* start = (const dex_u1*)pOptHeader + pOptHeader->depsOffset;
    const dex_u1* end = (const dex_u1*)pOptHeader + pOptHeader->optOffset +
                        pOptHeader->optLength;
    return (dex_u4)rd_adler32(start, end - start);
}

/* (documented in header file) */
bool dexParseOptData(const dex_u1* data, size_t length, DexFile* pDexFile) {
    const void* pOptStart = data + pDexFile->pOptHeader->optOffset;
    const void* pOptEnd = data + length;
    const dex_u4* pOpt = (const dex_u4*)pOptStart;
    dex_u4 optLength = (const dex_u1*)pOptEnd - (const dex_u1*)pOptStart;

    /*
     * Make sure the opt data start is in range and aligned. This may
     * seem like a superfluous check, but (a) if the file got
     * truncated, it might turn out that pOpt >= pOptEnd; and (b)
     * if the opt data header got corrupted, pOpt might not be
     * properly aligned. This test will catch both of these cases.
     */
    if(!isValidPointer(pOpt, pOptStart, pOptEnd)) {
        ALOGE("Bogus opt data start pointer");
        return false;
    }

    /* Make sure that the opt data length is a whole number of words. */
    if((optLength & 3) != 0) {
        ALOGE("Unaligned opt data area end");
        return false;
    }

    /*
     * Make sure that the opt data area is large enough to have at least
     * one chunk header.
     */
    if(optLength < 8) {
        ALOGE("Undersized opt data area (%u)", optLength);
        return false;
    }

    /* Process chunks until we see the end marker. */
    while(*pOpt != kDexChunkEnd) {
        if(!isValidPointer(pOpt + 2, pOptStart, pOptEnd)) {
            const dex_u4 offset = ((const dex_u1*)pOpt) - data;
            ALOGE("Bogus opt data content pointer at offset %u", offset);
            return false;
        }

        dex_u4 size = *(pOpt + 1);
        const dex_u1* pOptData = (const dex_u1*)(pOpt + 2);

        /*
         * The rounded size is 64-bit aligned and includes +8 for the
         * type/size header (which was extracted immediately above).
         */
        dex_u4 roundedSize = (size + 8 + 7) & ~7;
        const dex_u4* pNextOpt = pOpt + (roundedSize / sizeof(dex_u4));

        if(!isValidPointer(pNextOpt, pOptStart, pOptEnd)) {
            const dex_u4 offset = ((const dex_u1*)pOpt) - data;
            ALOGE("Opt data area problem for chunk of size %u at offset %u",
                  size, offset);
            return false;
        }

        switch(*pOpt) {
            case kDexChunkClassLookup:
                pDexFile->pClassLookup = (const DexClassLookup*)pOptData;
                break;
            case kDexChunkRegisterMaps:
                ALOGV("+++ found register maps, size=%u", size);
                pDexFile->pRegisterMapPool = pOptData;
                break;
            default:
                ALOGI(
                    "Unknown chunk 0x%08x (%c%c%c%c), size=%d in opt data area",
                    *pOpt, (char)((*pOpt) >> 24), (char)((*pOpt) >> 16),
                    (char)((*pOpt) >> 8), (char)(*pOpt), size);
                break;
        }

        pOpt = pNextOpt;
    }

    return true;
}
