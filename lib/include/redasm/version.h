#pragma once

#define _REDASM_XSTR(x) #x
#define _REDASM_STR(x) _REDASM_XSTR(x)

#define REDASM_API_LEVEL 1

#define REDASM_VERSION_MAJOR 4
#define REDASM_VERSION_MINOR 0
#define REDASM_VERSION_REV 0

#define REDASM_VERSION_STR                                                     \
    _REDASM_STR(REDASM_VERSION_MAJOR)                                          \
    "." _REDASM_STR(REDASM_VERSION_MINOR) "." _REDASM_STR(REDASM_VERSION_REV)
