import redasm
from . import header as PEH


def image_first_section(pe):
    return pe.dosheader.e_lfanew + pe.ntheaders.FileHeader.SizeOfOptionalHeader + 0x18


def aligned(v, align):
    valign = align

    while valign < v:
        valign = valign + align

    return valign


def is_code_section(s):
    return s and (s.Characteristics & PEH.IMAGE_SCN_CNT_CODE or
                  s.Characteristics & PEH.IMAGE_SCN_MEM_EXECUTE)


def rva_to_section(pe, rva):
    for section in pe.sections:
        if rva >= section.VirtualAddress and (rva < section.VirtualAddress + section.VirtualSize):
            return section

    return None


def rva_to_va(pe, rva):
    va = pe.optionalheader.ImageBase + rva
    return va if redasm.is_address(va) else None


def rva_to_offset(pe, rva):
    section = rva_to_section(pe, rva)

    if section:
        if section.VirtualSize == 0:
            return None

        offset = section.PointerToRawData + (rva - section.VirtualAddress)

        if offset > (section.PointerToRawData + section.SizeOfRawData):
            return None

        return offset

    return None


def register_common_types():
    redasm.create_struct("IMAGE_DOS_HEADER", PEH.IMAGE_DOS_HEADER)
    redasm.create_struct("IMAGE_FILE_HEADER", PEH.IMAGE_FILE_HEADER)
    redasm.create_struct("IMAGE_DATA_DIRECTORY", PEH.IMAGE_DATA_DIRECTORY)
    redasm.create_struct("IMAGE_NT_HEADERS", PEH.IMAGE_NT_HEADERS)
    redasm.create_struct("IMAGE_SECTION_HEADER", PEH.IMAGE_SECTION_HEADER)
    redasm.create_struct("IMAGE_EXPORT_DIRECTORY", PEH.IMAGE_EXPORT_DIRECTORY)
    redasm.create_struct("IMAGE_IMPORT_DESCRIPTOR",
                         PEH.IMAGE_IMPORT_DESCRIPTOR)
