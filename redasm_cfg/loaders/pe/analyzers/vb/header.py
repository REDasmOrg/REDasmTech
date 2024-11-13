VB_HEADER = (
    ("char[4]", "szVbMagic"),
    ("u16", "wRuntimeBuild"),          # Build of the VB6 Runtime
    ("char", "szLangDll[14]"),         # Language Extension DLL
    ("char", "szSecLangDll[14]"),      # 2nd Language Extension DLL
    ("u16", "wRuntimeRevision"),       # Internal Runtime Revision
    ("u32", "dwLCID"),                 # LCID of Language DLL
    ("u32", "dwSecLCID"),              # LCID of 2nd Language DLL
    ("u32", "lpSubMain"),              # Pointer to Sub Main Code
    ("u32", "lpProjectData"),          # Pointer to Project Data
    ("u32", "fMdlIntCtls"),            # VB Control Flags for IDs < 32
    ("u32", "fMdlIntCtls2"),           # VB Control Flags for IDs > 32
    ("u32", "dwThreadFlags"),          # Threading Mode
    ("u32", "dwThreadCount"),          # Threads to support in pool
    ("u16", "wFormCount"),             # Number of forms present
    ("u16", "wExternalCount"),         # Number of external controls
    ("u32", "dwThunkCount"),           # Number of thunks to create
    ("u32", "lpGuiTable"),             # Pointer to GUI Table
    ("u32", "lpExternalCompTable"),    # Pointer to External Component Table
    ("u32", "lpComRegisterData"),      # Pointer to COM Information
    ("u32", "bszProjectDescription"),  # Project Description (rel:VB_HEADER)
    ("u32", "bszProjectExeName"),      # Project EXE Name (rel:VB_HEADER)
    ("u32", "bszProjectHelpFile"),     # Project Help File (rel:VB_HEADER)
    ("u32", "bszProjectName"),         # Project Name (rel:VB_HEADER)
)
