import redasm
from enum import IntEnum, auto
from loaders.pe.resources import PEResourceId
from loaders.pe import borland as Borland


class PEClassification(IntEnum):
    UNCLASSIFIED = 0
    MINGW = auto()

    VISUALBASIC_5 = auto()
    VISUALBASIC_6 = auto()

    VISUALSTUDIO = auto()
    VISUALSTUDIO_2002 = auto()
    VISUALSTUDIO_2003 = auto()
    VISUALSTUDIO_2005 = auto()
    VISUALSTUDIO_2008 = auto()
    VISUALSTUDIO_2010 = auto()
    VISUALSTUDIO_2012 = auto()
    VISUALSTUDIO_2013 = auto()
    VISUALSTUDIO_2015 = auto()
    VISUALSTUDIO_2017 = auto()

    DOTNET_1 = auto()
    DOTNET = auto()

    BORLAND_DELPHI = auto()
    BORLAND_DELPHI_3 = auto()
    BORLAND_DELPHI_6 = auto()
    BORLAND_DELPHI_7 = auto()
    BORLAND_DELPHI_9_10 = auto()
    BORLAND_DELPHI_XE = auto()
    BORLAND_DELPHI_XE2_6 = auto()
    BORLAND_CPP = auto()


def check_visualbasic(c):
    if c in [PEClassification.VISUALBASIC_5, PEClassification.VISUALBASIC_6]:
        return c

    return PEClassification.UNCLASSIFIED


def check_delphi(c):
    if (c >= PEClassification.BORLAND_DELPHI and c <= PEClassification.BORLAND_DELPHI_XE2_6):
        return c

    return PEClassification.UNCLASSIFIED


def check_dotnet(c):
    if c in [PEClassification.DOTNET_1, PEClassification.DOTNET]:
        return c

    return PEClassification.UNCLASSIFIED


def check_visualstudio(c):
    if (c >= PEClassification.VISUALSTUDIO and c <= PEClassification.VISUALSTUDIO_2017):
        return c

    return PEClassification.UNCLASSIFIED


def check_borland(c):
    if (check_delphi(c) or c == PEClassification.BORLAND_CPP):
        return c

    return PEClassification.UNCLASSIFIED


def is_dotnet(c):
    return check_dotnet(c) != PEClassification.UNCLASSIFIED


def is_visualbasic(c):
    return check_visualbasic(c) != PEClassification.UNCLASSIFIED


def is_borland(c):
    return check_borland(c) != PEClassification.UNCLASSIFIED


def is_delphi(c):
    return check_delphi(c) != PEClassification.UNCLASSIFIED


class PEClassifier:
    def __init__(self):
        self.value = PEClassification.UNCLASSIFIED

    @property
    def is_classified(self):
        return self.value != PEClassification.UNCLASSIFIED

    def classify_import(self, library):
        library = library.lower()

        if library.startswith("msvbvm50"):
            self.value = PEClassification.VISUALBASIC_5
        elif library.startswith("msvbvm60"):
            self.value = PEClassification.VISUALBASIC_6
        elif library.startswith("libstdc++"):
            self.value = PEClassification.MINGW

        if ((is_visualbasic(self.value) or self.is_classified) and
                (self.value != PEClassification.VISUALSTUDIO)):
            return

        if library.startswith("msvcp50"):
            self.value = PEClassification.VISUALSTUDIO_5
        elif library.startswith("msvcp60") or library.startswith("msvcrt."):
            self.value = PEClassification.VISUALSTUDIO_6
        elif library.startswith("msvcp70") or library.startswith("msvcr70"):
            self.value = PEClassification.VISUALSTUDIO_2002
        elif library.startswith("msvcp71") or library.startswith("msvcr71"):
            self.value = PEClassification.VISUALSTUDIO_2003
        elif library.startswith("msvcp80") or library.startswith("msvcr80"):
            self.value = PEClassification.VISUALSTUDIO_2005
        elif library.startswith("msvcp90") or library.startswith("msvcr90"):
            self.value = PEClassification.VISUALSTUDIO_2008
        elif library.startswith("msvcp100") or library.startswith("msvcr100"):
            self.value = PEClassification.VISUALSTUDIO_2010
        elif library.startswith("msvcp110") or library.startswith("msvcr110"):
            self.value = PEClassification.VISUALSTUDIO_2012
        elif library.startswith("msvcp120") or library.startswith("msvcr120"):
            self.value = PEClassification.VISUALSTUDIO_2013
        elif library.startswith("msvcp140") or library.startswith("vcruntime140"):
            self.value = PEClassification.VISUALSTUDIO_2015

    def classify_borland(self, pe):
        rcdata = pe.resources.find(PEResourceId.RCDATA)
        if not rcdata:
            return

        pkginfodir = rcdata.find("PACKAGEINFO")
        if not pkginfodir:
            return

        Borland.register_types()

        entries = pkginfodir.get_entries()
        if entries:
            address, size = pe.resources.entry_dataaddress(entries[0])
            if address:
                packageinfo = redasm.set_type(address, "PACKAGEINFO")

                if Borland.is_delphi(packageinfo):
                    SIGNATURES = {
                        "delphi3": PEClassification.BORLAND_DELPHI_3,
                        "delphiXE2_6": PEClassification.BORLAND_DELPHI_XE2_6,
                        "delphiXE": PEClassification.BORLAND_DELPHI_XE,
                        "delphi9_10": PEClassification.BORLAND_DELPHI_9_10,
                        "delphi6": PEClassification.BORLAND_DELPHI_6,
                        "delphi7": PEClassification.BORLAND_DELPHI_7,
                    }

                    self.value = SIGNATURES.get(Borland.get_signature(address, size),
                                                         PEClassification.BORLAND_DELPHI)
                elif Borland.is_cpp(packageinfo):
                    self.value = PEClassification.BORLAND_CPP
