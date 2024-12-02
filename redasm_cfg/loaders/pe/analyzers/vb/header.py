VB_HEADER = (
    ("char[4]", "szVbMagic"),               # "VB5!"
    ("u16", "wRuntimeBuild"),               # Build of the VB6 Runtime
    ("char[14]", "szLangDll"),              # Language Extension DLL
    ("char[14]", "szSecLangDll"),           # 2nd Language Extension DLL
    ("u16", "wRuntimeRevision"),            # Internal Runtime Revision
    ("u32", "dwLCID"),                      # LCID of Language DLL
    ("u32", "dwSecLCID"),                   # LCID of 2nd Language DLL
    ("u32", "lpSubMain"),                  # Pointer to Sub Main Code
    ("u32", "lpProjectData"),              # Pointer to Project Data
    ("u32", "fMdlIntCtls"),                 # VB Control Flags for IDs < 32
    ("u32", "fMdlIntCtls2"),                # VB Control Flags for IDs > 32
    ("u32", "dwThreadFlags"),               # Threading Mode
    ("u32", "dwThreadCount"),               # Threads to support in pool
    ("u16", "wFormCount"),                  # Number of forms present
    ("u16", "wExternalCount"),              # Number of external controls
    ("u32", "dwThunkCount"),                # Number of thunks to create
    ("u32", "lpGuiTable"),                 # Pointer to GUI Table
    ("u32", "lpExternalCompTable"),        # Pointer to External Component Table
    ("u32", "lpComRegisterData"),          # Pointer to COM Information
    ("u32", "bSZProjectDescription"),       # Project Description (rel:VB_HEADER)
    ("u32", "bSZProjectExeName"),           # Project EXE Name (rel:VB_HEADER)
    ("u32", "bSZProjectHelpFile"),          # Project Help File (rel:VB_HEADER)
    ("u32", "bSZProjectName"),              # Project Name (rel:VB_HEADER)
)

COM_REGISTRATION_DATA = (
    ("u32", "bRegInfo"),                    # Offset to COM Interfaces Info
    ("u32", "bSZProjectName"),              # Offset to Project/Typelib Name
    ("u32", "bSZHelpDirectory"),            # Offset to Help Directory
    ("u32", "bSZProjectDescription"),       # Offset to Project Description
    ("u8[16]", "uuidProjectClsId"),         # CLSID of Project/Typelib
    ("u32", "dwTlbLcid"),                   # LCID of Type Library
    ("u32", "wUnknown"),
    ("u32", "wTlbVerMajor"),                # Typelib Major Version
    ("u32", "wTlbVerMinor"),                # Typelib Minor Version
)

COM_REGISTRATION_INFO = (
    ("u32", "bNextObject"),
    ("u32", "bObjectName"),
    ("u32", "bObjectDescription"),
    ("u32", "dwInstancing"),
    ("u32", "dwObjectId"),
    ("u8[16]", "uuidObject"),
    ("u32", "fIsInterface"),
    ("u32", "bUidObjectIFace"),
    ("u32", "bUidEventsIFace"),
    ("u32", "fHasEvents"),
    ("u32", "dwMiscStatus"),
    ("u8", "fClassType"),
    ("u8", "fObjectType"),
    ("u16", "wToolboxBitmap32"),
    ("u16", "wDefaultIcon"),
    ("u16", "fIsDesigner"),
    ("u32", "bDesignerData"),
)

VB_PROJECT_INFO = (
    ("u32", "dwVersion"),                 # Version
    ("u32", "lpObjectTable"),            # Pointer to the Object Table
    ("u32", "dwNull"),                    # Unused value after compilation.
    ("u32", "lpCodeStart"),              # Points to start of code (Unused)
    ("u32", "lpCodeEnd"),                # Points to end of code (Unused)
    ("u32", "dwDataSize"),                # Size of VB Object Structures (Unused)
    ("u32", "lpThreadSpace"),            # Pointer to Pointer to Thread Object
    ("u32", "lpVbaSeh"),                 # Pointer to VBA Exception Handler
    ("u32", "lpNativeCode"),             # Pointer to .DATA section
    ("wchar[264]", "szPathInformation"),  # Contains Path and ID string. < SP6
    ("u32", "lpExternalTable"),          # Pointer to External Table
    ("u32", "dwExternalCount"),           # Objects in the External Table
)

VB_PROJECT_INFO2 = (
    ("u32", "lpHeapLink"),           # Unused after compilation, always 0
    ("u32", "lpObjectTable"),        # Back-Pointer to the Object Table
    ("u32", "dwReserved"),            # Always set to -1 after compiling (Unused)
    ("u32", "dwUnused"),              # Not written or read in any case
    ("u32", "lpObjectList"),         # Pointer to Object Descriptor Pointer
    ("u32", "dwUnused2"),             # Not written or read in any case.
    ("u32", "szProjectDescription"),  # Pointer to Project Description
    ("u32", "szProjectHelpFile"),     # Pointer to Project Help File
    ("u32", "dwReserved2"),           # Always set to -1 after compiling. (Unused)
    ("u32", "dwHelpContextId"),       # Help Context ID set in Project Settings
)

# VB_GUI_TABLE = (
#     ("u32", "lpSectionHeader"),  # Address of section header
#     ("u8[59]", "dwReserved"),    # Unused Bytes
#     ("u32", "dwFormSize"),       # Block size, describing the form and its controls
#     ("u32", "dwReserved1"),
#     ("u32", "lpFormData"),       # Pointer to the block describing the form and its controls
#     ("u32", "dwReserved2"),
# )

VB_OBJECT_TABLE = (
    ("u32", "lpHeapLink"),        # Always set to 0 after compiling (Unused)
    ("u32", "lpExecProj"),       # Pointer to VB Project Exec COM Object
    ("u32", "lpProjectInfo2"),
    ("u32", "dwReserved"),        # Always set to -1 after compiling (Unused)
    ("u32", "dwNull"),
    ("u32", "lpProjectObject"),  # Pointer to in-memory Project Data
    ("GUID", "uuidObject"),       # GUID of the Object Table
    ("u16", "fCompileState"),     # Internal flag used during compilation
    ("u16", "wTotalObjects"),     # Total objects present in Project
    ("u16", "wCompiledObjects"),  # Equal to above after compiling
    ("u16", "wObjectsInUse"),     # Usually equal to above after compile
    ("u32", "lpObjectArray"),    # Pointer to Public Object Descriptors
    ("u32", "fIdeFlag"),          # Flag/Pointer used in IDE only
    ("u32", "lpIdeData"),         # Flag/Pointer used in IDE only
    ("u32", "lpIdeData2"),        # Flag/Pointer used in IDE only
    ("u32", "lpszProjectName"),  # Pointer to Project Name
    ("u32", "dwLcid"),            # LCID of Project
    ("u32", "dwLcid2"),           # Alternate LCID of Project
    ("u32", "lpIdeData3"),        # Flag/Pointer used in IDE only
    ("u32", "dwIdentifier"),      # Template Version of Structure
)

VB_PRIVATE_OBJECT_DESCRIPTOR = (
    ("u32", "lpHeapLink"),
    ("u32", "lpObjectInfo"),
    ("u32", "dwReserved"),
    ("u32", "dwIdeData"),
    ("u32", "dwReserved1"),
    ("u32", "dwReserved2"),
    ("u32", "lpObjectList"),
    ("u32", "dwIdeData2"),
    ("u32", "lpObjectList2"),
    ("u32", "dwUnknown1"),
    ("u32", "dwUnknown2"),
    ("u32", "dwIdeData3"),
    ("u32", "dwReserved4"),
    ("u32", "dwReserved5"),
    ("u32", "dwObjectType"),
    ("u32", "dwIdentifier"),
)

VB_OBJECT_INFO = (
    ("u16", "wRefCount"),         # Always 1 after compilation
    ("u16", "wObjectIndex"),      # Index of this Object in the public descriptor array
    ("u32", "lpObjectTable"),    # Pointer to the Object Table
    ("u32", "lpIdeData"),        # Zero after compilation (Used in IDE only)
    ("u32", "lpPrivateObject"),  # Pointer to Private Object Descriptor
    ("u32", "dwReserved"),        # Always -1 after compilation
    ("u32", "dwNull"),            # Unused
    ("u32", "lpObject"),         # Back-Pointer to Public Object Descriptor
    ("u32", "lpProjectData"),    # Pointer to in-memory Project Object
    ("u16", "wMethodCount"),      # Number of Methods
    ("u16", "wMethodCount2"),     # Zeroed out after compilation (IDE only)
    ("u32", "lpMethods"),        # Pointer to Array of Methods
    ("u16", "wConstants"),        # Number of Constants in Constant Pool
    ("u16", "wMaxConstants"),     # Constants to allocate in Constant Pool
    ("u32", "lpIdeData2"),       # Valid in IDE only
    ("u32", "lpIdeData3"),       # Valid in IDE only
    ("u32", "lpConstants"),      # Pointer to Constants Pool
)

VB_OBJECT_INFO_OPTIONAL = (
    ("VB_OBJECT_INFO", "base"),
    ("u32", "dwObjectGuids"),        # How many GUIDs to Register. 2 = Designer
    ("u32", "lpObjectGuid"),        # Unique GUID of the Object?
    ("u32", "dwNull"),               # Unused
    ("u32", "lpuuidObjectTypes"),   # Pointer to Array of Object Interface GUIDs
    ("u32", "dwObjectTypeGuids"),    # How many GUIDs in the Array above
    ("u32", "lpControls2"),         # Usually the same as lpControls
    ("u32", "dwNull2"),              # Unused
    ("u32", "lpObjectGuid2"),       # Pointer to Array of Object GUIDs
    ("u32", "dwControlCount"),       # Number of Controls in array below
    ("u32", "lpControls"),          # Pointer to Controls Array
    ("u16", "wEventCount"),          # Number of Events in Event Array
    ("u16", "wPCodeCount"),          # Number of P-Codes used by this Object
    ("u16", "bWInitializeEvent"),    # Offset to Initialize Event from Event Table
    ("u16", "bWTerminateEvent"),     # Offset to Terminate Event in Event Table
    ("u32", "lpEvents"),            # Pointer to Events Array
    ("u32", "lpBasicClassObject"),  # Pointer to in-memory Class Objects
    ("u32", "dwNull3"),              # Unused
    ("u32", "lpIdeData"),           # Only valid in IDE
)

VB_PUBLIC_OBJECT_DESCRIPTOR = (
    ("u32", "lpObjectInfo"),    # Pointer to the Object Info for this Object
    ("u32", "dwReserved"),       # -1
    ("u32", "lpPublicBytes"),   # Pointer to Public Variable Size integers
    ("u32", "lpStaticBytes"),   # Pointer to Static Variable Size integers
    ("u32", "lpModulePublic"),  # Pointer to Public Variables in DATA section
    ("u32", "lpModuleStatic"),  # Pointer to Static Variables in DATA section
    ("u32", "lpszObjectName"),  # Name of the Object
    ("u32", "dwMethodCount"),    # Number of Methods in Object
    ("u32", "lpMethodNames"),   # If present, pointer to Method names array
    ("u32", "bStaticVars"),      # Offset to where to copy Static Variables
    ("u32", "fObjectType"),      # Flags defining the Object Type
    ("u32", "dwNull"),           # Not valid after compilation
)

VB_CONTROL_INFO = (
    ("u16", "wFlagImplement"),        # FlagImplement
    ("u16", "wEventHandlerCount"),    # Number of Events Handlers
    ("u16", "wFlagIndexRef"),         # Flag2
    ("u16", "bWEventsOffset"),        # Offset in to Memory struct to copy Events
    ("u32", "lpGuid"),               # Pointer to GUID of this Control
    ("u32", "dwIndex"),               # Index ID of this Control
    ("u32", "dwNull"),                # Unused
    ("u32", "dwNull2"),               # Unused
    ("u32", "lpEventHandlerTable"),  # Pointer to Event Handler Table
    ("u32", "lpIdeData"),            # Valid in IDE only
    ("u32", "lpszName"),             # Name of this Control
    ("u32", "dwIndexCopy"),           # Secondary Index ID of this Control
)

VB_EVENT_INFO = (
    ("u32", "dwNull"),
    ("u32", "lpControls"),              # back-pointer to VbControlInfo
    ("u32", "lpObjectInfo"),            # back-pointer to VbObjectInfoOptional
    ("u32", "lpEVENT_SINK_QueryInterface"),
    ("u32", "lpEVENT_SINK_AddRef"),
    ("u32", "lpEVENT_SINK_Release"),
    # ("u32[1]", "lpEvents")
)
