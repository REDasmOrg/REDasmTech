IMAGE_SIZEOF_SHORT_NAME = 8
IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16
IMAGE_DOS_SIGNATURE = 0x5a4d
IMAGE_NT_SIGNATURE = 0x00004550
IMAGE_NT_OPTIONAL_HDR32_MAGIC = 0x10b
IMAGE_NT_OPTIONAL_HDR64_MAGIC = 0x20b
IMAGE_ROM_OPTIONAL_HDR_MAGIC = 0x107
IMAGE_ORDINAL_FLAG64 = 0x8000000000000000
IMAGE_ORDINAL_FLAG32 = 0x80000000

UNW_FLAG_MASK = 0x0F  # Lua Specific
UNW_FLAG_NHANDLER = 0
UNW_FLAG_EHANDLER = 1
UNW_FLAG_UHANDLER = 2
UNW_FLAG_CHAININFO = 4

# Sections
IMAGE_SCN_MEM_DISCARDABLE = 0x02000000
IMAGE_SCN_MEM_EXECUTE = 0x20000000
IMAGE_SCN_MEM_READ = 0x40000000
IMAGE_SCN_MEM_WRITE = 0x80000000
IMAGE_SCN_CNT_CODE = 0x00000020
IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040
IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x00000080

# Data Directory
IMAGE_DIRECTORY_ENTRY_EXPORT = 0
IMAGE_DIRECTORY_ENTRY_IMPORT = 1
IMAGE_DIRECTORY_ENTRY_RESOURCE = 2
IMAGE_DIRECTORY_ENTRY_EXCEPTION = 3
IMAGE_DIRECTORY_ENTRY_SECURITY = 4
IMAGE_DIRECTORY_ENTRY_BASERELOC = 5
IMAGE_DIRECTORY_ENTRY_DEBUG = 6
IMAGE_DIRECTORY_ENTRY_ARCHITECTURE = 7
IMAGE_DIRECTORY_ENTRY_GLOBALPTR = 8
IMAGE_DIRECTORY_ENTRY_TLS = 9
IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG = 10
IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT = 11
IMAGE_DIRECTORY_ENTRY_IAT = 12
IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT = 13
IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR = 14
IMAGE_DIRECTORY_ENTRY_DOTNET = IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR

# Architecture
IMAGE_FILE_MACHINE_AM33 = 0x1d3
IMAGE_FILE_MACHINE_AMD64 = 0x8664
IMAGE_FILE_MACHINE_ARM = 0x1c0
IMAGE_FILE_MACHINE_EBC = 0xebc
IMAGE_FILE_MACHINE_I386 = 0x14c
IMAGE_FILE_MACHINE_IA64 = 0x200
IMAGE_FILE_MACHINE_M32R = 0x9041
IMAGE_FILE_MACHINE_MIPS16 = 0x266
IMAGE_FILE_MACHINE_MIPSFPU = 0x366
IMAGE_FILE_MACHINE_MIPSFPU16 = 0x466
IMAGE_FILE_MACHINE_POWERPC = 0x1f0
IMAGE_FILE_MACHINE_POWERPCFP = 0x1f1
IMAGE_FILE_MACHINE_R4000 = 0x166
IMAGE_FILE_MACHINE_SH3 = 0x1a2
IMAGE_FILE_MACHINE_SH3E = 0x01a4
IMAGE_FILE_MACHINE_SH3DSP = 0x1a3
IMAGE_FILE_MACHINE_SH4 = 0x1a6
IMAGE_FILE_MACHINE_SH5 = 0x1a8
IMAGE_FILE_MACHINE_THUMB = 0x1c2
IMAGE_FILE_MACHINE_WCEMIPSV2 = 0x169
IMAGE_FILE_MACHINE_R3000 = 0x162
IMAGE_FILE_MACHINE_R10000 = 0x168
IMAGE_FILE_MACHINE_ALPHA = 0x184
IMAGE_FILE_MACHINE_ALPHA64 = 0x0284
IMAGE_FILE_MACHINE_AXP64 = IMAGE_FILE_MACHINE_ALPHA64
IMAGE_FILE_MACHINE_CEE = 0xC0EE
IMAGE_FILE_MACHINE_TRICORE = 0x0520
IMAGE_FILE_MACHINE_CEF = 0x0CEF

IMAGE_DOS_HEADER = (
    ("u16", "e_magic"),
    ("u16", "e_cblp"),
    ("u16", "e_cp"),
    ("u16", "e_crlc"),
    ("u16", "e_cparhdr"),
    ("u16", "e_minalloc"),
    ("u16", "e_maxalloc"),
    ("u16", "e_ss"),
    ("u16", "e_sp"),
    ("u16", "e_csum"),
    ("u16", "e_ip"),
    ("u16", "e_cs"),
    ("u16", "e_lfarlc"),
    ("u16", "e_ovno"),
    ("u16[4]", "e_res"),
    ("u16", "e_oemid"),
    ("u16", "e_oeminfo"),
    ("u16[10]", "e_res2"),
    ("u32", "e_lfanew"),
)

IMAGE_FILE_HEADER = (
    ("u16", "Machine"),
    ("u16", "NumberOfSections"),
    ("u32", "TimeDateStamp"),
    ("u32", "PointerToSymbolTable"),
    ("u32", "NumberOfSymbols"),
    ("u16", "SizeOfOptionalHeader"),
    ("u16", "Characteristics"),
)

IMAGE_NT_HEADERS = (
    ("u32", "Signature"),
    ("IMAGE_FILE_HEADER", "FileHeader"),
)

IMAGE_DATA_DIRECTORY = (
    ("u32^", "VirtualAddress"),
    ("u32", "Size"),
)

IMAGE_OPTIONAL_HEADER32 = (
    ("u16", "Magic"),
    ("u8", "MajorLinkerVersion"),
    ("u8", "MinorLinkerVersion"),
    ("u32", "SizeOfCode"),
    ("u32", "SizeOfInitializedData"),
    ("u32", "SizeOfUninitializedData"),
    ("u32^", "AddressOfEntryPoint"),
    ("u32", "BaseOfCode"),
    ("u32", "BaseOfData"),
    ("u32", "ImageBase"),
    ("u32", "SectionAlignment"),
    ("u32", "FileAlignment"),
    ("u16", "MajorOperatingSystemVersion"),
    ("u16", "MinorOperatingSystemVersion"),
    ("u16", "MajorImageVersion"),
    ("u16", "MinorImageVersion"),
    ("u16", "MajorSubsystemVersion"),
    ("u16", "MinorSubsystemVersion"),
    ("u32", "Win32VersionValue"),
    ("u32", "SizeOfImage"),
    ("u32", "SizeOfHeaders"),
    ("u32", "CheckSum"),
    ("u16", "Subsystem"),
    ("u16", "DllCharacteristics"),
    ("u32", "SizeOfStackReserve"),
    ("u32", "SizeOfStackCommit"),
    ("u32", "SizeOfHeapReserve"),
    ("u32", "SizeOfHeapCommit"),
    ("u32", "LoaderFlags"),
    ("u32", "NumberOfRvaAndSizes"),
    ("IMAGE_DATA_DIRECTORY[16]", "DataDirectory"),
)

IMAGE_OPTIONAL_HEADER64 = (
    ("u16", "Magic"),
    ("u8", "MajorLinkerVersion"),
    ("u8", "MinorLinkerVersion"),
    ("u32", "SizeOfCode"),
    ("u32", "SizeOfInitializedData"),
    ("u32", "SizeOfUninitializedData"),
    ("u32^", "AddressOfEntryPoint"),
    ("u32", "BaseOfCode"),
    ("u64", "ImageBase"),
    ("u32", "SectionAlignment"),
    ("u32", "FileAlignment"),
    ("u16", "MajorOperatingSystemVersion"),
    ("u16", "MinorOperatingSystemVersion"),
    ("u16", "MajorImageVersion"),
    ("u16", "MinorImageVersion"),
    ("u16", "MajorSubsystemVersion"),
    ("u16", "MinorSubsystemVersion"),
    ("u32", "Win32VersionValue"),
    ("u32", "SizeOfImage"),
    ("u32", "SizeOfHeaders"),
    ("u32", "CheckSum"),
    ("u16", "Subsystem"),
    ("u16", "DllCharacteristics"),
    ("u64", "SizeOfStackReserve"),
    ("u64", "SizeOfStackCommit"),
    ("u64", "SizeOfHeapReserve"),
    ("u64", "SizeOfHeapCommit"),
    ("u32", "LoaderFlags"),
    ("u32", "NumberOfRvaAndSizes"),
    ("IMAGE_DATA_DIRECTORY[16]", "DataDirectory"),
)

IMAGE_SECTION_HEADER = (
    ("char[8]", "Name"),
    ("u32", "VirtualSize"),
    ("u32", "VirtualAddress"),
    ("u32", "SizeOfRawData"),
    ("u32", "PointerToRawData"),
    ("u32", "PointerToRelocations"),
    ("u32", "PointerToLinenumbers"),
    ("u16", "NumberOfRelocations"),
    ("u16", "NumberOfLinenumbers"),
    ("u32", "Characteristics"),
)

IMAGE_EXPORT_DIRECTORY = (
    ("u32", "Characteristics"),
    ("u32", "TimeDateStamp"),
    ("u16", "MajorVersion"),
    ("u16", "MinorVersion"),
    ("u32^", "Name"),
    ("u32", "Base"),
    ("u32", "NumberOfFunctions"),
    ("u32", "NumberOfNames"),
    ("u32^", "AddressOfFunctions"),
    ("u32^", "AddressOfNames"),
    ("u32^", "AddressOfNameOrdinals"),
)

IMAGE_IMPORT_DESCRIPTOR = (
    ("u32^", "OriginalFirstThunk"),
    ("u32", "TimeDateStamp"),
    ("u32", "ForwarderChain"),
    ("u32^", "Name"),
    ("u32^", "FirstThunk"),
)

IMAGE_RESOURCE_DIRECTORY = (
    ("u32", "Characteristics"),
    ("u32", "TimeDateStamp"),
    ("u16", "MajorVersion"),
    ("u16", "MinorVersion"),
    ("u16", "NumberOfNamedEntries"),
    ("u16", "NumberOfIdEntries"),
)

IMAGE_RESOURCE_DIRECTORY_ENTRY = (
    ("u32", "Id"),
    ("u32", "Offset"),
)

IMAGE_RUNTIME_FUNCTION_ENTRY = (
    ("u32^", "BeginAddress"),
    ("u32", "EndAddress"),
    ("u32", "UnwindInfoAddress"),
)


UNWIND_INFO = (
    ("u8", "VersionAndFlags"),
    ("u8", "SizeOfProlog"),
    ("u8", "CountOfCodes"),
    ("u8", "FrameRegisterAndOffset"),
)
