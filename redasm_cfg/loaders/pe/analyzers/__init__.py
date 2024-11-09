import redasm
from .wndproc import wndproc_isenabled, wndproc_execute


def register_analyzers(pe):
    redasm.register_analyzer(
        name="Analyze Window Procedures",
        description="Find and Disassemble Window Procedure",
        flags=redasm.ANALYZER_SELECTED,
        is_enabled=lambda: wndproc_isenabled(pe),
        execute=lambda: wndproc_execute(pe),
    )
