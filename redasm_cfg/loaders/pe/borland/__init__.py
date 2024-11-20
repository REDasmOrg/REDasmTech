"""
 Package flags:
  bit     meaning
  -----------------------------------------------------------------------------------------
  0     | 1: never-build                  0: always build
  1     | 1: design-time only             0: not design-time only      on => bit 2 = off
  2     | 1: run-time only                0: not run-time only         on => bit 1 = off
  3     | 1: do not check for dup units   0: perform normal dup unit check
  4..25 | reserved
  26..27| (producer) 0: pre-V4, 1: undefined, 2: c++, 3: Pascal
  28..29| reserved
  30..31| 0: EXE, 1: Package DLL, 2: Library DLL, 3: undefined
"""

import redasm
from . import header as BH


def _packageinfo_contains(pkginfoaddr, size, q):
    data = redasm.get_type(pkginfoaddr, f"u8[{size}]")
    if not data:
        return False

    return bytes(data).find(q.encode()) != -1


def get_producer_flag(packageinfo):
    return (packageinfo.flags & 0x0C000000) >> 26


def is_pre_v4(packageinfo):
    return get_producer_flag(packageinfo) == 0


def is_cpp(packageinfo):
    return get_producer_flag(packageinfo) == 2


def is_pascal(packageinfo):
    return get_producer_flag(packageinfo) == 3


def is_delphi(packageinfo):
    return is_pascal(packageinfo) or is_pre_v4(packageinfo)


def get_signature(pkginfoaddr, size):
    packageinfo = redasm.get_type(pkginfoaddr, "PACKAGEINFO")

    if packageinfo:
        if is_pre_v4(packageinfo):
            return "delphi3"
        if _packageinfo_contains(pkginfoaddr, size, "System.SysUtils"):
            return "delphiXE2_6"
        if _packageinfo_contains(pkginfoaddr, size, "ExcUtils"):
            return "delphiXE"
        if _packageinfo_contains(pkginfoaddr, size, "StrUtils"):
            return "delphi9_10"
        if _packageinfo_contains(pkginfoaddr, size, "ImageHlp"):
            return "delphi6"
        if _packageinfo_contains(pkginfoaddr, size, "SysInit"):
            return "delphi7"

    return None


def register_types():
    redasm.create_struct("PACKAGEINFO", BH.PACKAGEINFO)
