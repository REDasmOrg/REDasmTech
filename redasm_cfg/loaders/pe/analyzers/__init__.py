import redasm
from .wndproc import wndproc_isenabled, wndproc_execute
from .vb import vb_isenabled, vb_execute


def register_analyzers(pe):
    redasm.register_analyzer(
        name="Analyze Window Procedures",
        description="Find and Disassemble Window Procedure",
        flags=redasm.ANALYZER_SELECTED,
        is_enabled=lambda: wndproc_isenabled(pe),
        execute=lambda: wndproc_execute(pe),
    )

    redasm.register_analyzer(
        name="Decompile VB5/6",
        description="Find and and Decompile Visual Basic events",
        flags=redasm.ANALYZER_SELECTED | redasm.ANALYZER_RUNONCE,
        is_enabled=lambda: vb_isenabled(pe),
        execute=lambda: vb_execute(pe),
    )
