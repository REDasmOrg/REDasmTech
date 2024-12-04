import redasm
from . import format as PE
from . import header as PEH
from . import debug as Debug
from .classifier import PEClassifier
from .resources import PEResources
from .analyzers import register_analyzers


class PEFormat:
    def __init__(self):
        self.classifier = PEClassifier()
        self.imagebase = 0
        self.dosheader = None
        self.ntheaders = None
        self.optionalheader = None

    def get_import_name(self, library, name):
        return f"{library}.{name}"


def check_header():
    PE.register_types()
    pe = PEFormat()

    stream = redasm.FileStream()
    pe.dosheader = stream.collect_type("IMAGE_DOS_HEADER")
    if not pe.dosheader or pe.dosheader.e_magic != PEH.IMAGE_DOS_SIGNATURE:
        return None

    stream.seek(pe.dosheader.e_lfanew)
    pe.ntheaders = stream.collect_type("IMAGE_NT_HEADERS")
    if pe.ntheaders.Signature != PEH.IMAGE_NT_SIGNATURE:
        return None

    magic = stream.peek_u16()

    if magic == PEH.IMAGE_NT_OPTIONAL_HDR32_MAGIC:
        redasm.set_bits(32)
        redasm.create_struct("IMAGE_OPTIONAL_HEADER",
                             PEH.IMAGE_OPTIONAL_HEADER32)
    elif magic == PEH.IMAGE_NT_OPTIONAL_HDR64_MAGIC:
        redasm.set_bits(64)
        redasm.create_struct("IMAGE_OPTIONAL_HEADER",
                             PEH.IMAGE_OPTIONAL_HEADER64)
    else:
        raise "Invalid OptionalHeader Magic"

    pe.integertype = "u32" if redasm.get_bits() == 32 else "u64"
    pe.optionalheader = stream.collect_type("IMAGE_OPTIONAL_HEADER")
    pe.imagebase = pe.optionalheader.ImageBase
    return pe


def check_dotnet(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_DOTNET]
    if entry.VirtualAddress == 0:
        return None

    va = redasm.from_reladdress(entry.VirtualAddress)
    if not va:
        return None

    entry = redasm.get_type(va, "IMAGE_COR_HEADER")
    # TODO: Dotnet parsing
    return entry


def read_sections(pe):
    t = f"IMAGE_SECTION_HEADER[{pe.ntheaders.FileHeader.NumberOfSections}]"
    pe.sections = redasm.file.map_type(PE.image_first_section(pe), t)


def read_exports(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_EXPORT]
    if entry.VirtualAddress == 0:
        return

    va = redasm.from_reladdress(entry.VirtualAddress)
    if not va:
        return

    exporttable = redasm.set_type(va, "IMAGE_EXPORT_DIRECTORY")
    if not exporttable:
        return

    functionsva = redasm.from_reladdress(exporttable.AddressOfFunctions)
    namesva = redasm.from_reladdress(exporttable.AddressOfNames)
    ordinalsva = redasm.from_reladdress(exporttable.AddressOfNameOrdinals)

    if not functionsva or not namesva or not ordinalsva:
        print("Corrupted Export Table")
        return

    nameva = redasm.from_reladdress(exporttable.Name)

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
                nameaddr = redasm.from_reladdress(names[j])
                ordinal = exporttable.Base + ord
                break

        if nameaddr:
            name = redasm.set_type(nameaddr, "str")
        else:
            name = f"Ordinal__{redasm.to_hex(ordinal, 4)}"

        redasm.set_entry(redasm.from_reladdress(f), name)


def read_imports(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_IMPORT]
    if entry.VirtualAddress == 0:
        return

    va = redasm.from_reladdress(entry.VirtualAddress)
    if not va:
        return

    ORDINAL_FLAG = PEH.IMAGE_ORDINAL_FLAG64 if redasm.get_bits() == 64 else PEH.IMAGE_ORDINAL_FLAG32
    entry = redasm.set_type(va, "IMAGE_IMPORT_DESCRIPTOR")

    while entry and (entry.FirstThunk != 0 or entry.OriginalFirstThunk != 0):
        moduleva = redasm.from_reladdress(entry.Name)

        if moduleva:
            modulename = redasm.set_type(moduleva, "str").lower()
            pe.classifier.classify_import(modulename)

            thunkstart = redasm.from_reladdress(entry.OriginalFirstThunk or entry.FirstThunk)
            iatstart = redasm.from_reladdress(entry.FirstThunk)

            if thunkstart:
                nthunks, currva = 0, thunkstart
                thunk = redasm.get_type(currva, pe.integertype)

                while thunk != 0:
                    importname = ""

                    # Instructions refers to FT
                    iatva = iatstart + (nthunks * redasm.size_of(pe.integertype))

                    if thunk & ORDINAL_FLAG:
                        ordinal = thunk ^ ORDINAL_FLAG
                        importname = pe.get_import_name(modulename, f"ordinal_{ordinal}")
                    else:
                        importbynameva = redasm.from_reladdress(thunk)

                        if importbynameva:
                            name = redasm.set_type(importbynameva + redasm.size_of("u16"), "str")
                            importname = pe.get_import_name(modulename, name)
                            redasm.set_type(importbynameva, "u16")
                            redasm.set_name(importbynameva, f"{importname}_hint")

                    redasm.set_type(iatva, pe.integertype)
                    redasm.set_name_ex(iatva, importname, redasm.SN_IMPORT)

                    currva = currva + redasm.size_of(pe.integertype)
                    nthunks = nthunks + 1
                    thunk = redasm.get_type(currva, pe.integertype)

        va = va + redasm.size_of("IMAGE_IMPORT_DESCRIPTOR")
        entry = redasm.set_type(va, "IMAGE_IMPORT_DESCRIPTOR")


def read_exceptions(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_EXCEPTION]
    if entry.VirtualAddress == 0:
        return

    va = redasm.from_reladdress(entry.VirtualAddress)
    if not va:
        return

    n = entry.Size // redasm.size_of("IMAGE_RUNTIME_FUNCTION_ENTRY")
    entries = redasm.set_type(va, f"IMAGE_RUNTIME_FUNCTION_ENTRY[{n}]")

    for e in entries:
        if e.UnwindInfoAddress & 1:
            continue

        unwindva = redasm.from_reladdress(e.UnwindInfoAddress)
        if not unwindva:
            continue

        unwindinfo = redasm.set_type(unwindva, "UNWIND_INFO")
        flags = unwindinfo.VersionAndFlags >> 3
        va = redasm.from_reladdress(e.BeginAddress)

        if unwindinfo and not (flags & PEH.UNW_FLAG_CHAININFO):
            redasm.set_function(va)


def read_debuginfo(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_DEBUG]
    if entry.VirtualAddress == 0:
        return

    va = redasm.from_reladdress(entry.VirtualAddress)
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
            dbgva = PE.redasm.from_reladdress(dir.AddressOfRawData)

        if not dbgva and dbgdir.PointerToRawData:
            dbgva = redasm.to_address(dbgdir.PointerToRawData)

        if not dbgva:
            continue

        if dbgdir.Type == PEH.IMAGE_DEBUG_TYPE_CODEVIEW:
            redasm.log("Debug info type: CODEVIEW")
            pe.classifier.check_visualstudio()

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


def read_resources(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_RESOURCE]
    if entry.VirtualAddress == 0:
        return

    va = redasm.from_reladdress(entry.VirtualAddress)
    if va:
        pe.resources = PEResources(pe, va)
        pe.classifier.classify_borland(pe)


def select_processor(pe):
    machine = pe.ntheaders.FileHeader.Machine

    if machine == PEH.IMAGE_FILE_MACHINE_I386:
        redasm.set_processor("x86_32")
    elif machine == PEH.IMAGE_FILE_MACHINE_AMD64:
        redasm.set_processor("x86_64")
    elif machine == PEH.IMAGE_FILE_MACHINE_ARM:
        if pe.optionalheader.Magic == PEH.IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            redasm.set_processor("arm64le")
        else:
            redasm.set_processor("arm32le")


def load_default(pe):
    read_exports(pe)
    read_imports(pe)
    read_exceptions(pe)
    # read_debuginfo(pe)
    read_resources(pe)
    select_processor(pe)

    redasm.set_entry(redasm.from_reladdress(pe.optionalheader.AddressOfEntryPoint),
                     "PE_EntryPoint")


def init():
    pe = check_header()
    if not pe:
        return False

    imagebase = pe.optionalheader.ImageBase
    sectionalign = pe.optionalheader.SectionAlignment
    filealign = pe.optionalheader.FileAlignment
    ep = pe.optionalheader.AddressOfEntryPoint

    redasm.memory.map(imagebase, PE.aligned(
        pe.optionalheader.SizeOfImage, sectionalign))

    read_sections(pe)

    for sect in pe.sections:
        startva = imagebase + sect.VirtualAddress
        asize = PE.aligned(sect.VirtualSize, sectionalign)
        startoff = sect.PointerToRawData
        osize = PE.aligned(
            sect.SizeOfRawData, filealign) if startoff > 0 else 0

        segtype = redasm.SEG_UNKNOWN

        if (sect.Characteristics & PEH.IMAGE_SCN_CNT_CODE) or (sect.Characteristics & PEH.IMAGE_SCN_MEM_EXECUTE):
            segtype |= redasm.SEG_HASCODE
        elif ep and (ep >= sect.VirtualAddress and ep < sect.VirtualAddress + asize):
            segtype |= redasm.SE_HASCODE

        if (sect.Characteristics & PEH.IMAGE_SCN_CNT_INITIALIZED_DATA) or (sect.Characteristics & PEH.IMAGE_SCN_CNT_UNINITIALIZED_DATA):
            segtype |= redasm.SEG_HASDATA

        redasm.map_segment_n(sect.Name, startva, asize,
                             startoff, osize, segtype)

    corheader = check_dotnet(pe)

    if corheader:
        print("DOTNET WIP")
    else:
        load_default(pe)

    register_analyzers(pe)
    return True


redasm.register_loader(
    id="pe",
    name="Portable Executable",
    init=init
)
