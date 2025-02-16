import redasm
from loaders.pe import format as PE
from loaders.pe import header as PEH
from loaders.pe import debug as Debug
from loaders.pe.classifier import PEClassifier
from loaders.pe.resources import PEResources
from loaders.pe.analyzers import register_pe_analyzers


class PELoader:
    id = "pe"
    name = "Portable Executable"

    def __init__(self):
        self.classifier = PEClassifier()
        self.imagebase = 0
        self.dosheader = None
        self.ntheaders = None
        self.optionalheader = None

    def from_rva(self, rva):
        return self.imagebase + rva

    def check_dotnet(self):
        entry = self.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_DOTNET]
        if entry.VirtualAddress == 0:
            return None

        va = self.from_rva(entry.VirtualAddress)
        if not va:
            return None

        entry = redasm.get_type(va, "IMAGE_COR_HEADER")
        # TODO: Dotnet parsing
        return entry

    def read_exports(self):
        entry = self.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_EXPORT]
        if entry.VirtualAddress == 0:
            return

        va = self.from_rva(entry.VirtualAddress)
        if not va:
            return

        exporttable = redasm.set_type(va, "IMAGE_EXPORT_DIRECTORY")
        if not exporttable:
            return

        functionsva = self.from_rva(exporttable.AddressOfFunctions)
        namesva = self.from_rva(exporttable.AddressOfNames)
        ordinalsva = self.from_rva(exporttable.AddressOfNameOrdinals)

        if not functionsva or not namesva or not ordinalsva:
            print("Corrupted Export Table")
            return

        nameva = self.from_rva(exporttable.Name)

        if nameva:
            redasm.set_type(nameva, "str")

        functions = redasm.set_type(
            functionsva, f"u32[{exporttable.NumberOfFunctions}]")
        names = redasm.set_type(namesva, f"u32[{exporttable.NumberOfNames}]")
        ordinals = redasm.set_type(ordinalsva, f"u16[{exporttable.NumberOfNames}]")

        for i, f in enumerate(functions):
            if not f:
                continue

            nameaddr = None
            ordinal = None

            for j, ord in enumerate(ordinals):
                if ord == i:
                    nameaddr = self.from_rva(names[j])
                    ordinal = exporttable.Base + ord
                    break

            if nameaddr:
                name = redasm.set_type(nameaddr, "str")
            else:
                name = f"Ordinal__{redasm.to_hex(ordinal, 4)}"

            redasm.set_entry(self.from_rva(f), name)

    def read_imports(self):
        entry = self.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_IMPORT]
        if entry.VirtualAddress == 0:
            return

        va = self.from_rva(entry.VirtualAddress)
        if not va:
            return

        ORDINAL_FLAG = PEH.IMAGE_ORDINAL_FLAG64 if self.bits == 64 else PEH.IMAGE_ORDINAL_FLAG32
        entry = redasm.set_type(va, "IMAGE_IMPORT_DESCRIPTOR")

        while entry and (entry.FirstThunk != 0 or entry.OriginalFirstThunk != 0):
            moduleva = self.from_rva(entry.Name)

            if moduleva:
                modulename = redasm.set_type(moduleva, "str").lower()
                self.classifier.classify_import(modulename)

                thunkstart = self.from_rva(entry.OriginalFirstThunk or entry.FirstThunk)
                iatstart = self.from_rva(entry.FirstThunk)

                if thunkstart:
                    nthunks, currva = 0, thunkstart
                    thunk = redasm.get_type(currva, self.integertype)

                    while thunk != 0:
                        importname = ""

                        # Instructions refers to FT
                        iatva = iatstart + (nthunks * redasm.size_of(self.integertype))

                        if thunk & ORDINAL_FLAG:
                            ordinal = thunk ^ ORDINAL_FLAG
                            importname = PE.get_import_name(modulename, f"ordinal_{ordinal}")
                        else:
                            importbynameva = self.from_rva(thunk)

                            if importbynameva:
                                name = redasm.set_type(importbynameva + redasm.size_of("u16"), "str")
                                importname = PE.get_import_name(modulename, name)
                                redasm.set_type(importbynameva, "u16")
                                redasm.set_name(importbynameva, f"{importname}_hint")

                        redasm.set_type(iatva, self.integertype)
                        redasm.set_name_ex(iatva, importname, redasm.SN_IMPORT)

                        currva = currva + redasm.size_of(self.integertype)
                        nthunks = nthunks + 1
                        thunk = redasm.get_type(currva, self.integertype)

            va = va + redasm.size_of("IMAGE_IMPORT_DESCRIPTOR")
            entry = redasm.set_type(va, "IMAGE_IMPORT_DESCRIPTOR")

    def read_exceptions(self):
        entry = self.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_EXCEPTION]
        if entry.VirtualAddress == 0:
            return

        va = self.from_rva(entry.VirtualAddress)
        if not va:
            return

        n = entry.Size // redasm.size_of("IMAGE_RUNTIME_FUNCTION_ENTRY")
        entries = redasm.set_type(va, f"IMAGE_RUNTIME_FUNCTION_ENTRY[{n}]")

        for e in entries:
            if e.UnwindInfoAddress & 1:
                continue

            unwindva = self.from_rva(e.UnwindInfoAddress)
            if not unwindva:
                continue

            unwindinfo = redasm.set_type(unwindva, "UNWIND_INFO")
            flags = unwindinfo.VersionAndFlags >> 3
            va = self.from_rva(e.BeginAddress)

            if unwindinfo and not (flags & PEH.UNW_FLAG_CHAININFO):
                redasm.set_function(va)
                redasm.set_name(va, f"rnt_entry_{redasm.to_hex(va, n=-1)}")

    def read_debuginfo(self):
        entry = self.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_DEBUG]
        if entry.VirtualAddress == 0:
            return

        va = self.from_rva(entry.VirtualAddress)
        if not va:
            return

        n = entry.Size // redasm.size_of("IMAGE_DEBUG_DIRECTORY")
        if n == 0:
            return

        Debug.register_debug_types()

        dbgdirs = redasm.set_type(va, f"IMAGE_DEBUG_DIRECTORY[{n}]")
        if not dbgdirs:
            return

        for dbgdir in dbgdirs:
            dbgva = 0

            if dbgdir.AddressOfRawData:
                dbgva = PE.self.from_rva(dbgdir.AddressOfRawData)

            if not dbgva and dbgdir.PointerToRawData:
                dbgva = redasm.to_address(dbgdir.PointerToRawData)

            if not dbgva:
                continue

            if dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_CODEVIEW:
                redasm.log("Debug info type: CODEVIEW")
                # pe.classifier.check_visualstudio()

                cvheader = redasm.get_type(dbgva, "CV_HEADER")

                if cvheader.Signature == Debug.PE_CVINFO_PDB20_SIGNATURE:
                    redasm.set_type(dbgva, "CV_INFO_PDB20")
                    pdbfilename = redasm.set_type(dbgva + redasm.size_of("CV_INFO_PDB20"), "str")
                    redasm.log(f"PDB 2.0 @ '{pdbfilename}'")
                elif cvheader.Signature == Debug.PE_CVINFO_PDB70_SIGNATURE:
                    redasm.set_type(dbgva, "CV_INFO_PDB70")
                    pdbfilename = redasm.set_type(dbgva + redasm.size_of("CV_INFO_PDB70"), "str")
                    redasm.log(f"PDB 7.0 @ '{pdbfilename}'")
                else:
                    redasm.log(f"Unknown signature '{cvheader.Signature}'")

            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_UNKNOWN:
                redasm.log("Debug info type: UNKNOWN")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_COFF:
                redasm.log("Debug info type: COFF")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_FPO:
                redasm.log("Debug info type: FPO")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_MISC:
                redasm.log("Debug info type: Misc")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_EXCEPTION:
                redasm.log("Debug info type: Exception")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_FIXUP:
                redasm.log("Debug info type: FixUp")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_OMAP_TO_SRC:
                redasm.log("Debug info type: OMAP to Src")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_OMAP_FROM_SRC:
                redasm.log("Debug info type: OMAP from Src")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_BORLAND:
                redasm.log("Debug info type: Borland")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_RESERVED10:
                redasm.log("Debug info type: Reserved10")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_CLSID:
                redasm.log("Debug info type: CLSID")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_VC_FEATURE:
                redasm.log("Debug info type: VC Feature")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_POGO:
                redasm.log("Debug info type: POGO")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_ILTCG:
                redasm.log("Debug info type: ILTCG")
            elif dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_REPRO:
                redasm.log("Debug info type: REPRO")
            else:
                redasm.log(f"Unknown Debug info type (value {redasm.to_hex(dbgdir.Type)})")

    def read_resources(self):
        entry = self.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_RESOURCE]
        if entry.VirtualAddress == 0:
            return

        va = self.from_rva(entry.VirtualAddress)
        if va:
            self.resources = PEResources(self, va)
            self.classifier.classify_borland(self)

    def load_default(self):
        self.read_exports()
        self.read_imports()
        self.read_exceptions()
        self.read_debuginfo()
        self.read_resources()

        redasm.set_entry(self.from_rva(self.optionalheader.AddressOfEntryPoint),
                         "PE_EntryPoint")

    def parse(self, req):
        stream = redasm.Stream(req.file)

        self.dosheader = stream.read_struct(PEH.IMAGE_DOS_HEADER)
        if not self.dosheader or self.dosheader.e_magic != PEH.IMAGE_DOS_SIGNATURE:
            return False

        stream.seek(self.dosheader.e_lfanew)
        signature = stream.peek_u32()
        if signature != PEH.IMAGE_NT_SIGNATURE:
            return False

        PE.register_types()
        self.ntheaders = stream.read_type("IMAGE_NT_HEADERS")

        magic = stream.peek_u16()
        if magic == PEH.IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            self.bits = 32
            self.integertype = "u32"
            redasm.create_struct("IMAGE_OPTIONAL_HEADER", PEH.IMAGE_OPTIONAL_HEADER32)
        elif magic == PEH.IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            self.bits = 64
            self.integertype = "u64"
            redasm.create_struct("IMAGE_OPTIONAL_HEADER", PEH.IMAGE_OPTIONAL_HEADER64)
        else:
            return False

        self.optionalheader = stream.read_type("IMAGE_OPTIONAL_HEADER")
        if not self.optionalheader:
            return False

        self.imagebase = self.optionalheader.ImageBase

        t = f"IMAGE_SECTION_HEADER[{self.ntheaders.FileHeader.NumberOfSections}]"
        stream.seek(PE.image_first_section(self))
        self.sections = stream.read_type(t)
        return self.sections is not None

    def load(self, file):
        imagebase = self.optionalheader.ImageBase
        sectionalign = self.optionalheader.SectionAlignment
        filealign = self.optionalheader.FileAlignment
        ep = self.optionalheader.AddressOfEntryPoint

        for sect in self.sections:
            startva = imagebase + sect.VirtualAddress
            asize = PE.aligned(sect.VirtualSize, sectionalign)
            startoff = sect.PointerToRawData
            osize = PE.aligned(
                sect.SizeOfRawData, filealign) if startoff > 0 else 0

            perm = 0

            if (sect.Characteristics & PEH.IMAGE_SCN_MEM_EXECUTE or
                    (ep and (ep >= sect.VirtualAddress and ep < sect.VirtualAddress + asize))):
                perm |= redasm.SP_X
            if sect.Characteristics & PEH.IMAGE_SCN_MEM_READ:
                perm |= redasm.SP_R
            if sect.Characteristics & PEH.IMAGE_SCN_MEM_WRITE:
                perm |= redasm.SP_W

            redasm.add_segment_n(sect.Name, startva, asize, perm, self.bits)

            if osize:
                redasm.map_file_n(startoff, startva, osize)

        corheader = self.check_dotnet()

        if corheader:
            print("DOTNET WIP")
        else:
            self.load_default()

        return True

    def get_processor(self):
        if self.check_dotnet():
            return "cil"

        machine = self.ntheaders.FileHeader.Machine

        if machine == PEH.IMAGE_FILE_MACHINE_I386:
            return "x86_32"
        elif machine == PEH.IMAGE_FILE_MACHINE_AMD64:
            return "x86_64"
        elif machine == PEH.IMAGE_FILE_MACHINE_ARM:
            if self.optionalheader.Magic == PEH.IMAGE_NT_OPTIONAL_HDR64_MAGIC:
                return "arm64le"
            else:
                return "arm32le"

        return None


redasm.register_loader(PELoader)
register_pe_analyzers()
