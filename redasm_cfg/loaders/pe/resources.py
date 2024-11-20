import redasm
from . import format as PE
from enum import IntEnum


class PEResourceId(IntEnum):
    CURSORS = 1,
    BITMAPS = 2,
    ICONS = 3,
    MENUS = 4,
    DIALOGS = 5,
    STRING_TABLES = 6,
    FONT_DIRECTORY = 7,
    FONTS = 8,
    ACCELERATORS = 9,
    RCDATA = 10,
    MESSAGE_TABLES = 11,
    CURSOR_GROUPS = 12,
    ICON_GROUPS = 14,
    VERSION_INFO = 16,
    HTML_PAGES = 23,
    CONFIGURATION_FILES = 24,


# PE_RESOURCE_TYPES = {
#     1: "CURSORS",
#     2: "BITMAPS",
#     3: "ICONS",
#     4: "MENUS",
#     5: "DIALOGS",
#     6: "STRING_TABLES",
#     7: "FONT_DIRECTORY",
#     8: "FONTS",
#     9: "ACCELERATORS",
#     10: "RCDATA",
#     11: "MESSAGE_TABLES",
#     12: "CURSOR_GROUPS",
#     14: "ICON_GROUPS",
#     16: "VERSION_INFO",
#     23: "HTML_PAGES",
#     24: "CONFIGURATION_FILES",
# }

class PEResourceDir:
    def __init__(self, address, peresources):
        self.address = address
        self.resources = peresources

    def get_resource_dir(self):
        return redasm.get_type(self.address, "IMAGE_RESOURCE_DIRECTORY")

    def get_entries(self):
        resdir = self.get_resource_dir()
        c = resdir.NumberOfIdEntries + resdir.NumberOfNamedEntries

        if c > 0:
            return redasm.get_type(self.address + redasm.size_of("IMAGE_RESOURCE_DIRECTORY"),
                                   f"IMAGE_RESOURCE_DIRECTORY_ENTRY[{c}]")

        return []

    def find(self, name):
        for e in self.get_entries():
            if self.resources.entry_getname(e) != name:
                continue

            if self.resources.entry_isdirectory(e):
                return PEResourceDir(self.resources.rootaddress + self.resources.entry_directoryoffset(e),
                                     self.resources)
            else:
                return e

        return None


class PEResources:
    def __init__(self, pe, address):
        self.pe = pe
        self.rootaddress = address
        self.read_resource_dir(address)

    def read_resource_dir(self, address):
        resdir = redasm.set_type(address, "IMAGE_RESOURCE_DIRECTORY")
        c = resdir.NumberOfIdEntries + resdir.NumberOfNamedEntries
        if c == 0:
            return

        direntries = redasm.set_type(address + redasm.size_of("IMAGE_RESOURCE_DIRECTORY"),
                                     f"IMAGE_RESOURCE_DIRECTORY_ENTRY[{c}]")
        for e in direntries:
            if self.entry_nameisstring(e):
                eaddr = self.rootaddress + self.entry_nameoffset(e)
                namelen = redasm.set_type(eaddr, "u16")
                redasm.set_type(eaddr + redasm.size_of("u16"), f"wchar[{namelen}]")

            if self.entry_isdirectory(e):
                self.read_resource_dir(self.rootaddress + self.entry_directoryoffset(e))
            else:
                redasm.set_type(self.rootaddress + e.Offset, "IMAGE_RESOURCE_DATA_ENTRY")

    def find(self, name):
        return PEResourceDir(self.rootaddress, self).find(name)

    def entry_isdirectory(self, e):
        return (e.Offset & 0x80000000) != 0

    def entry_nameisstring(self, e):
        return (e.Id & 0x80000000) != 0

    def entry_nameoffset(self, e):
        return e.Id & ~0x80000000

    def entry_directoryoffset(self, e):
        return e.Offset & ~0x80000000

    def entry_getname(self, e):
        if self.entry_nameisstring(e):
            eaddr = self.rootaddress + self.entry_nameoffset(e)
            namelen = redasm.set_type(eaddr, "u16")
            return redasm.get_type(eaddr + redasm.size_of("u16"), f"wchar[{namelen}]")

        return e.Id & ~0x80000000

    def entry_dataaddress(self, e):
        if not self.entry_isdirectory(e):
            dataentry = redasm.get_type(self.rootaddress + e.Offset, "IMAGE_RESOURCE_DATA_ENTRY")
            if dataentry.OffsetToData:
                return (PE.rva_to_va(self.pe, dataentry.OffsetToData), dataentry.Size)

        return (None, None)
