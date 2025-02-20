#pragma once

#include <redasm/common.h>
#include <redasm/theme.h>
#include <redasm/types.h>

typedef enum RDSymbolKind {
    SYMBOL_INVALID = 0,
    SYMBOL_SEGMENT,
    SYMBOL_FUNCTION,
    SYMBOL_STRING,
    SYMBOL_TYPE,
} RDSymbolKind;

typedef enum RDListingItemType {
    LISTINGITEM_EMPTY = 0,
    LISTINGITEM_HEX_DUMP,
    LISTINGITEM_FILL,
    LISTINGITEM_INSTRUCTION,
    LISTINGITEM_LABEL,
    LISTINGITEM_SEGMENT,
    LISTINGITEM_FUNCTION,
    LISTINGITEM_TYPE,
} RDListingItemType;

typedef struct RDSymbol {
    RDAddress address;
    usize type;
    RDThemeKind theme;
    const char* value;
} RDSymbol;

REDASM_EXPORT bool rdlisting_getindex(RDAddress address, LIndex* idx);
REDASM_EXPORT bool rdlisting_getsymbol(usize idx, RDSymbol* symbol);
REDASM_EXPORT usize rdlisting_getsymbolslength();
REDASM_EXPORT bool rdlisting_getimport(usize idx, RDSymbol* symbol);
REDASM_EXPORT usize rdlisting_getimportslength();
REDASM_EXPORT bool rdlisting_getexport(usize idx, RDSymbol* symbol);
REDASM_EXPORT usize rdlisting_getexportslength();
REDASM_EXPORT usize rdlisting_getlength();
