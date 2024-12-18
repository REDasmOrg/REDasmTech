import redasm
from loaders.pe.format import get_import_name
from loaders.pe.classifier import is_borland, is_visualbasic, is_dotnet

WNDPROC_API = [
    {"arg": 3, "name": "DialogBoxA"},
    {"arg": 3, "name": "DialogBoxW"},
    {"arg": 3, "name": "DialogBoxParamA"},
    {"arg": 3, "name": "DialogBoxParamW"},
    {"arg": 3, "name": "DialogBoxIndirectParamA"},
    {"arg": 3, "name": "DialogBoxIndirectParamW"},
    {"arg": 3, "name": "CreateDialogA"},
    {"arg": 3, "name": "CreateDialogW"},
    {"arg": 3, "name": "CreateDialogParamA"},
    {"arg": 3, "name": "CreateDialogParamW"},
    {"arg": 3, "name": "CreateDialogIndirectParamA"},
    {"arg": 3, "name": "CreateDialogIndirectParamW"},
]


def get_import(name):
    impname = get_import_name("user32.dll", name)
    address = redasm.get_address("_" + impname)

    if not address:
        address = redasm.get_address(impname)

    return address


def get_api_refs(name):
    address = get_import(name)
    return redasm.get_refsto(address) if address else []


def wndproc_isenabled():
    c = redasm.get_userdata("pe_class")
    return c is not None and not (is_borland(c) or is_dotnet(c) or is_visualbasic(c))


def find_wndproc(address, argidx):
    func = redasm.rdil.create_function(address)
    if not func:
        return

    vstack = []

    for x in func:
        if x.expr.op == redasm.rdil.PUSH:
            vstack.insert(0, (x.address, x.expr.u))
        elif x.expr.op == redasm.rdil.CALL:
            if x.address == address and argidx < len(vstack):
                f, wndproc = vstack[argidx]

                if hasattr(wndproc, "addr"):
                    redasm.set_name(wndproc.addr, f"DialogProc_{redasm.to_hex(wndproc.addr)}")
                    redasm.add_ref(f, wndproc.addr, redasm.CR_CALL)

                break
            else:
                vstack.clear()


def wndproc_execute():
    for api in WNDPROC_API:
        for ref in get_api_refs(api["name"]):
            find_wndproc(ref.address, api["arg"])
