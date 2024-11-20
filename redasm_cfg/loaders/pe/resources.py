import redasm
from . import format as PE

PE_RESOURCE_TYPES = {
    1: "CURSORS",
    2: "BITMAPS",
    3: "ICONS",
    4: "MENUS",
    5: "DIALOGS",
    6: "STRING_TABLES",
    7: "FONT_DIRECTORY",
    8: "FONTS",
    9: "ACCELERATORS",
    10: "RCDATA",
    11: "MESSAGE_TABLES",
    12: "CURSOR_GROUPS",
    14: "ICON_GROUPS",
    16: "VERSION_INFO",
    23: "HTML_PAGES",
    24: "CONFIGURATION_FILES",
}


class PEResources:
    def __init__(self, pe, address):
        self.pe = pe
        self.rootaddress = address
        self.entries = {}
        self.read_resource_dir(address)
        print(self.entries)

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
                name = redasm.set_type(eaddr + redasm.size_of("u16"), f"wchar[{namelen}]")
            else:
                name = self.get_resource_id(e)

            if self.entry_isdirectory(e):
                self.read_resource_dir(self.rootaddress + self.entry_directoryoffset(e))
            else:
                redasm.set_type(self.rootaddress + e.Offset, "IMAGE_RESOURCE_DATA_ENTRY")
                # data = redasm.set_type(self.rootaddress + e.Offset, "IMAGE_RESOURCE_DATA_ENTRY")
                # dataaddr = PE.rva_to_va(self.pe, data.OffsetToData)
                # if dataaddr and data.Size:  # NOTE: Handle resource types?
                # redasm.set_type(dataaddr, f"u8[{data.Size}]")

            self.entries.setdefault(name, []).append(e)

    def get_resource_data(self, entries):
        res = []

        for e in entries:
            if not self.entry_isdirectory(e):
                data = redasm.get_type(self.rootaddress + e.Offset, "IMAGE_RESOURCE_DATA_ENTRY")
                dataaddr = PE.rva_to_va(self.pe, data.OffsetToData)
                if dataaddr and data.Size:
                    res.append((dataaddr, data.Size))

        return res

    def get_resource_id(self, e):
        resid = e.Id & ~0x80000000
        return PE_RESOURCE_TYPES.get(resid, resid)

    def get_entry_name(self, e):
        if self.entry_nameisstring(e):
            address = self.rootaddress + self.entry_nameoffset(e)
            namelen = redasm.set_type(address, "u16")
            return redasm.get_type(address + redasm.size_of("u16"), f"wchar[{namelen}]")

        return self.get_resource_id(e.Id & ~0x80000000)

    def entry_isdirectory(self, e):
        return (e.Offset & 0x80000000) != 0

    def entry_nameisstring(self, e):
        return (e.Id & 0x80000000) != 0

    def entry_nameoffset(self, e):
        return e.Id & ~0x80000000

    def entry_directoryoffset(self, e):
        return e.Offset & ~0x80000000
