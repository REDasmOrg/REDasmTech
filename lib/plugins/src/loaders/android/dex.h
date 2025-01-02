#pragma once

// https://github.com/JesusFreke/smali/wiki/TypesMethodsAndFields
// https://source.android.com/devices/tech/dalvik/dex-format
// https://android.googlesource.com/platform/dalvik

#include <redasm/redasm.h>

namespace dex {

bool init(RDLoader*);

}
