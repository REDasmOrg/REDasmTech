import redasm

PE_CVINFO_PDB70_SIGNATURE = "RSDS"
PE_CVINFO_PDB20_SIGNATURE = "NB10"
PE_CVINFO_CV50_SIGNATURE = "NB11"
PE_CVINFO_CV41_SIGNATURE = "NB09"

CV_HEADER = (
    ("char[4]", "Signature"),
    ("u32", "Offset"),
)

CV_INFO_PDB20 = (
    ("char[4]", "Signature"),
    ("u32", "Offset"),
    ("u32", "Signature"),
    ("u32", "Age"),
    # ("char[1]", "PdbFileName"),
)

CV_INFO_PDB70 = (
    ("char[4]", "CvSignature"),
    ("GUID", "Signature"),
    ("u32", "Age"),
    # ("char[1]", "PdbFileName"),
)


def register_debug_types():
    redasm.create_struct("CV_HEADER", CV_HEADER)
    redasm.create_struct("CV_INFO_PDB20", CV_INFO_PDB20)
    redasm.create_struct("CV_INFO_PDB70", CV_INFO_PDB70)
