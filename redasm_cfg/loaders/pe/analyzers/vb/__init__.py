import redasm
from . import header as VBH


def vb_isenabled(pe):
    return pe.classifier.is_visualbasic


def vb_execute(pe):
    redasm.create_struct("VB_HEADER", VBH.VB_HEADER)

    ep = redasm.get_ep()
    if ep is None:
        return

    func = redasm.rdil.create_function(ep)
    if (len(func) < 2 or (func[0].expr.op != redasm.rdil.PUSH and
                          func[1].expr.op != redasm.rdil.CALL)):
        return

    vbaddr = func[0].expr.u.addr
    signature = redasm.get_type(vbaddr, "char[4]")

    if signature != "VB5!":
        return

    vbheader = redasm.set_type(vbaddr, "VB_HEADER")
    print(hex(vbheader.bszProjectName))
