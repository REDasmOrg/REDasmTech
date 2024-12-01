import redasm
from .wndproc import wndproc_isenabled, wndproc_execute
from .vb import vb_isenabled, vb_execute


def register_analyzers(pe):
    redasm.register_analyzer(
        id="pe_wndproc",
        name="Analyze Window Procedures",
        description="Find and Disassemble Window Procedure",
        flags=redasm.ANA_SELECTED,
        is_enabled=lambda: wndproc_isenabled(pe),
        execute=lambda: wndproc_execute(pe),
    )

    redasm.register_analyzer(
        id="pe_vb6",
        name="Decompile VB5/6",
        description="Find and and Decompile Visual Basic events",
        flags=redasm.ANA_SELECTED | redasm.ANA_RUNONCE,
        is_enabled=lambda: vb_isenabled(pe),
        execute=lambda: vb_execute(pe),
    )
