#pragma once

// https://github.com/JesusFreke/smali/wiki/TypesMethodsAndFields
// https://source.android.com/devices/tech/dalvik/dex-format
// https://android.googlesource.com/platform/dalvik

#include <libdex/DexFile.h>
#include <optional>
#include <redasm/redasm.h>

namespace dex {

struct DexLoader {
    DexFile* dexfile;

    template<typename T>
    const T* get_pointer(RDAddress address) const {
        return reinterpret_cast<const T*>(this->dexfile->baseAddr + address);
    }

    std::optional<RDAddress> get_address(const void* p) const {
        if(!p) return std::nullopt;
        return reinterpret_cast<const u8*>(p) - this->dexfile->baseAddr;
    }
};

extern RDLoaderPlugin loader;

} // namespace dex
