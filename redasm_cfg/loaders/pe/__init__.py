import redasm
from . import format as PE
from . import header as PEH


class PEFormat:
    bits = 0
    imagebase = 0
    dosheader = None
    ntheaders = None
    optionalheader = None


def check_header():
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
        redasm.create_struct("IMAGE_OPTIONAL_HEADER",
                             PEH.IMAGE_OPTIONAL_HEADER32)
        pe.bits = 32
    elif magic == PEH.IMAGE_NT_OPTIONAL_HDR64_MAGIC:
        redasm.create_struct("IMAGE_OPTIONAL_HEADER",
                             PEH.IMAGE_OPTIONAL_HEADER64)
        pe.bits = 64
    else:
        raise "Invalid OptionalHeader Magic"

    pe.integertype = "u32" if pe.bits == 32 else "u64"
    pe.optionalheader = stream.collect_type("IMAGE_OPTIONAL_HEADER")
    pe.imagebase = pe.optionalheader.ImageBase
    return pe


def read_sections(pe):
    t = f"IMAGE_SECTION_HEADER[{pe.ntheaders.FileHeader.NumberOfSections}]"
    pe.sections = redasm.file.map_type(PE.image_first_section(pe), t)


def read_exports(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_EXPORT]
    if entry.VirtualAddress == 0:
        return

    va = PE.rva_to_va(pe, entry.VirtualAddress)
    if not va:
        return

    exporttable = redasm.set_type(va, "IMAGE_EXPORT_DIRECTORY")
    if not exporttable:
        return

    functionsva = PE.rva_to_va(pe, exporttable.AddressOfFunctions)
    namesva = PE.rva_to_va(pe, exporttable.AddressOfNames)
    ordinalsva = PE.rva_to_va(pe, exporttable.AddressOfNameOrdinals)

    if not functionsva or not namesva or not ordinalsva:
        print("Corrupted Export Table")
        return

    nameva = PE.rva_to_va(pe, exporttable.Name)

    if nameva:
        redasm.set_type(nameva, "str")

    functions = redasm.set_type(
        functionsva, f"u32^[{exporttable.NumberOfFunctions}]")
    names = redasm.set_type(namesva, f"u32^[{exporttable.NumberOfNames}]")
    ordinals = redasm.set_type(ordinalsva, f"u16[{exporttable.NumberOfNames}]")

    for i, f in enumerate(functions):
        if not f:
            continue

        nameaddr = None
        ordinal = None

        for j, ord in enumerate(ordinals):
            if ord == i:
                nameaddr = PE.rva_to_va(pe, names[j])
                ordinal = exporttable.Base + ord
                break

        if nameaddr:
            name = redasm.set_type(nameaddr, "str")
        else:
            name = f"Ordinal__{redasm.to_hex(ordinal, 4)}"

        ep = pe.optionalheader.ImageBase + f
        redasm.set_function_as(ep, name)
        redasm.set_export(ep)
        redasm.enqueue(ep)


def read_imports(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_IMPORT]
    if entry.VirtualAddress == 0:
        return

    va = PE.rva_to_va(pe, entry.VirtualAddress)
    if not va:
        return

    ORDINAL_FLAG = PEH.IMAGE_ORDINAL_FLAG32 if pe.bits == 32 else PEH.IMAGE_ORDINAL_FLAG64
    entry = redasm.set_type(va, "IMAGE_IMPORT_DESCRIPTOR")

    while entry and (entry.FirstThunk != 0 or entry.OriginalFirstThunk != 0):
        moduleva = PE.rva_to_va(pe, entry.Name)

        if moduleva:
            modulename = redasm.set_type(moduleva, "str")
            thunkstart = PE.rva_to_va(
                pe, entry.FirstThunk or entry.OriginalFirstThunk)

            if thunkstart:
                nthunks, currva = 0, thunkstart
                thunk = redasm.get_type(currva, pe.integertype)

                while thunk != 0:
                    importname = ""

                    if thunk & ORDINAL_FLAG:
                        ordinal = thunk ^ ORDINAL_FLAG
                        importname = f"{modulename}_ordinal_{hex(ordinal)}"
                    else:
                        importbynameva = PE.rva_to_va(pe, thunk)

                        if importbynameva:
                            name = redasm.set_type(
                                importbynameva + redasm.size_of("u16"), "str")
                            importname = f"{modulename}.{name}"
                            redasm.set_type_as(
                                importbynameva, "u16", f"{importname}_hint")

                    redasm.set_type_as(currva, pe.integertype, importname)

                    currva = currva + redasm.size_of(pe.integertype)
                    nthunks = nthunks + 1
                    thunk = redasm.get_type(currva, pe.integertype)

        va = va + redasm.size_of("IMAGE_IMPORT_DESCRIPTOR")
        entry = redasm.set_type(va, "IMAGE_IMPORT_DESCRIPTOR")


def read_exceptions(pe):
    entry = pe.optionalheader.DataDirectory[PEH.IMAGE_DIRECTORY_ENTRY_EXCEPTION]
    if entry.VirtualAddress == 0:
        return

    va = PE.rva_to_va(pe, entry.VirtualAddress)
    if not va:
        return

    n = int(entry.Size / redasm.size_of("IMAGE_RUNTIME_FUNCTION_ENTRY"))
    entries = redasm.set_type(va, f"IMAGE_RUNTIME_FUNCTION_ENTRY[{n}]")

    for e in entries:
        if e.UnwindInfoAddress & 1:
            continue

        va = pe.optionalheader.ImageBase + e.BeginAddress

        unwindva = PE.rva_to_va(pe, e.UnwindInfoAddress)
        if not unwindva:
            continue

        unwindinfo = redasm.set_type(unwindva, "UNWIND_INFO")
        flags = unwindinfo.VersionAndFlags >> 3

        if unwindinfo and not (flags & PEH.UNW_FLAG_CHAININFO):
            redasm.set_function(va)
            redasm.enqueue(va)


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


def init():
    PE.register_common_types()
    pe = check_header()
    if not pe:
        return False

    imagebase = pe.optionalheader.ImageBase
    sectionalign = pe.optionalheader.SectionAlignment
    filealign = pe.optionalheader.FileAlignment

    redasm.memory.map(imagebase, PE.aligned(
        pe.optionalheader.SizeOfImage, sectionalign))

    read_sections(pe)

    for sect in pe.sections:
        startva = imagebase + sect.VirtualAddress
        asize = PE.aligned(sect.VirtualSize, sectionalign)
        startoff = sect.PointerToRawData
        osize = PE.aligned(
            sect.SizeOfRawData, filealign) if startoff > 0 else 0

        segtype = redasm.SEGMENT_UNKNOWN

        if (sect.Characteristics & PEH.IMAGE_SCN_CNT_CODE) or (sect.Characteristics & PEH.IMAGE_SCN_MEM_EXECUTE):
            segtype |= redasm.SEGMENT_HASCODE

        if (sect.Characteristics & PEH.IMAGE_SCN_CNT_INITIALIZED_DATA) or (sect.Characteristics & PEH.IMAGE_SCN_CNT_UNINITIALIZED_DATA):
            segtype |= redasm.SEGMENT_HASDATA

        redasm.map_segment_n(sect.Name, startva, asize,
                             startoff, osize, segtype)

    read_exports(pe)
    read_imports(pe)
    read_exceptions(pe)
    select_processor(pe)

    ep = imagebase + pe.optionalheader.AddressOfEntryPoint
    redasm.set_function_as(ep, "__entry_point__")
    redasm.enqueue(ep)
    return True


redasm.register_loader(
    name="Portable Executable",
    init=init
)
