from enum import Enum, auto


class PEClassification(Enum):
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


class PEClassifier:
    def __init__(self):
        self._classification = PEClassification.UNCLASSIFIED

    @property
    def is_classified(self):
        return self._classification != PEClassification.UNCLASSIFIED

    @property
    def is_dotnet(self):
        return self.check_dotnet() != PEClassification.UNCLASSIFIED

    @property
    def is_visualbasic(self):
        return self.check_visualbasic() != PEClassification.UNCLASSIFIED

    @property
    def is_borland(self):
        return self.check_borland() != PEClassification.UNCLASSIFIED

    @property
    def is_delphi(self):
        return self.check_delphi() != PEClassification.UNCLASSIFIED

    def check_visualbasic(self):
        if self._classification in [PEClassification.VISUALBASIC_5,
                                    PEClassification.VISUALBASIC_6]:
            return self._classification

        return PEClassification.UNCLASSIFIED

    def check_delphi(self):
        if (self._classification >= PEClassification.BORLAND_DELPHI and
                self._classification <= PEClassification.BORLAND_DELPHI_XE2_6):
            return self._classification

        return PEClassification.UNCLASSIFIED

    def check_dotnet(self):
        if self._classification in [PEClassification.DOTNET_1,
                                    PEClassification.DOTNET]:
            return self._classification

        return PEClassification.UNCLASSIFIED

    def check_visualstudio(self):
        if (self._classification >= PEClassification.VISUALSTUDIO and
                self._classification <= PEClassification.VISUALSTUDIO_2017):
            return self._classification

        return PEClassification.UNCLASSIFIED

    def check_borland(self):
        if (self.check_delphi() or
                self._classification == PEClassification.BORLAND_CPP):
            return self._classification

        return PEClassification.UNCLASSIFIED

    def classify_import(self, library):
        library = library.lower()

        if library.startswith("msvbvm50"):
            self._classification = PEClassification.VISUALBASIC_5
        elif library.startswith("msvbvm60"):
            self._classification = PEClassification.VISUALBASIC_6
        elif library.startswith("libstdc++"):
            self._classification = PEClassification.MINGW

        if ((self.is_visualbasic or self.is_classified) and
                (self._classification != PEClassification.VISUALSTUDIO)):
            return

        if library.startswith("msvcp50"):
            self._classification = PEClassification.VISUALSTUDIO_5
        elif library.startswith("msvcp60") or library.startswith("msvcrt."):
            self._classification = PEClassification.VISUALSTUDIO_6
        elif library.startswith("msvcp70") or library.startswith("msvcr70"):
            self._classification = PEClassification.VISUALSTUDIO_2002
        elif library.startswith("msvcp71") or library.startswith("msvcr71"):
            self._classification = PEClassification.VISUALSTUDIO_2003
        elif library.startswith("msvcp80") or library.startswith("msvcr80"):
            self._classification = PEClassification.VISUALSTUDIO_2005
        elif library.startswith("msvcp90") or library.startswith("msvcr90"):
            self._classification = PEClassification.VISUALSTUDIO_2008
        elif library.startswith("msvcp100") or library.startswith("msvcr100"):
            self._classification = PEClassification.VISUALSTUDIO_2010
        elif library.startswith("msvcp110") or library.startswith("msvcr110"):
            self._classification = PEClassification.VISUALSTUDIO_2012
        elif library.startswith("msvcp120") or library.startswith("msvcr120"):
            self._classification = PEClassification.VISUALSTUDIO_2013
        elif library.startswith("msvcp140") or library.startswith("vcruntime140"):
            self._classification = PEClassification.VISUALSTUDIO_2015
