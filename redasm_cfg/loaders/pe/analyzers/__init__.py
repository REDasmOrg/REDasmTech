import redasm
from loaders.pe.analyzers.vb import VBAnalyzer
# from loaders.pe.analyzers.wndproc import WndProcAnalyzer


def register_pe_analyzers():
    # redasm.register_analyzer(WndProcAnalyzer)
    redasm.register_analyzer(VBAnalyzer)
