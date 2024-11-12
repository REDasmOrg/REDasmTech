import redasm

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


def get_import(pe, name):
    impname = pe.get_import_name("user32.dll", name)
    address = redasm.get_address("_" + impname)

    if not address:
        address = redasm.get_address(impname)

    return address


def get_api_refs(pe, name):
    address = get_import(pe, name)
    return redasm.get_refs(address) if address else []


def wndproc_isenabled(pe):
    c = pe.classifier
    return not (c.is_borland or c.is_dotnet or c.is_visualbasic)


def find_wndproc(address, argidx):
    pass


def wndproc_execute(pe):
    for api in WNDPROC_API:
        for ref in get_api_refs(pe, api["name"]):
            find_wndproc(ref.address, api["arg"])
