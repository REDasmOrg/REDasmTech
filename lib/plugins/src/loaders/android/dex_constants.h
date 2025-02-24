#pragma once

namespace dex {

enum DexAccessFlag {
    DAF_PUBLIC = 0x1,
    DAF_PRIVATE = 0x2,
    DAF_PROTECTED = 0x4,
    DAF_STATIC = 0x8,
    DAF_FINAL = 0x10,
    DAF_SYNCHRONIZED = 0x20,
    DAF_VOLATILE = 0x40,
    DAF_BRIDGE = 0x40,
    DAF_TRANSIENT = 0x80,
    DAF_VAR_ARGS = 0x80,
    DAF_NATIVE = 0x100,
    DAF_INTERFACE = 0x200,
    DAF_ABSTRACT = 0x400,
    DAF_STRICT = 0x800,
    DAF_SYNTHETIC = 0x1000,
    DAF_ANNOTATION = 0x2000,
    DAF_ENUM = 0x8000,
    DAF_CONSTRUCTOR = 0x10000,
    DAF_DECLARED_SYNCHRONIZED = 0x20000,
};

enum DexValueFormat {
    DVF_BYTE = 0x00,
    DVF_SHORT = 0x02,
    DVF_CHAR = 0x03,
    DVF_INT = 0x04,
    DVF_LONG = 0x06,
    DVF_FLOAT = 0x10,
    DVF_DOUBLE = 0x11,
    DVF_METHOD_TYPE = 0x15,
    DVF_METHOD_HANDLE = 0x16,

    DVF_STRING = 0x17,
    DVF_TYPE = 0x18,
    DVF_FIELD = 0x19,
    DVF_METHOD = 0x1a,
    DVF_ENUM = 0x1b,

    DVF_ARRAY = 0x1c,
    DVF_ANNOTATION = 0x1d,
    DVF_NULL = 0x1e,
    DVF_BOOLEAN = 0x1f,
};

enum DexTypeCode {
    DTC_HEADER = 0x0000,
    DTC_STRINGID = 0x0001,
    DTC_TYPEID = 0x0002,
    DTC_PROTOID = 0x0003,
    DTC_FIELDID = 0x0004,
    DTC_METHODID = 0x0005,
    DTC_CLASSDEF = 0x0006,
    DTC_CALLSITEID = 0x0007,
    DTC_METHODHANDLE = 0x0008,
    DTC_MAPLIST = 0x1000,
    DTC_TYPELIST = 0x1001,
    DTC_ANNOTATIONSETREFLIST = 0x1002,
    DTC_ANNOTATIONSET = 0x1003,
    DTC_CLASSDATA = 0x2000,
    DTC_CODE = 0x2001,
    DTC_STRINGDATA = 0x2002,
    DTC_DEBUGINFO = 0x2003,
    DTC_ANNOTATION = 0x2004,
    DTC_ENCODEDARRAY = 0x2005,
    DTC_ANNOTATIONSDIRECTORY = 0x2006,
};

} // namespace dex
