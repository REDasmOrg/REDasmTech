from loaders.pe.analyzers.vb import vb_isenabled, vb_execute
from loaders.pe.analyzers.wndproc import WndProcAnalyzer
import redasm


def register_pe_analyzers():
    redasm.register_analyzer(WndProcAnalyzer)
    # redasm.register_analyzer(
    #     id="pe_vb6",
    #     name="Decompile VB5/6",
    #     description="Find and and Decompile Visual Basic events",
    #     flags=redasm.ANA_SELECTED | redasm.ANA_RUNONCE,
    #     is_enabled=vb_isenabled,
    #     execute=vb_execute,
    # )
