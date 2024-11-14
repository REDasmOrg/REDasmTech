from types import SimpleNamespace
import redasm
from . import header as VBH


def has_optional_info(obj, objinfo):
    return ((obj.lpObjectInfo + redasm.size_of("VB_OBJECT_INFO")) !=
            objinfo.base.lpConstants)


def decompile_object(vb, obj):
    objname = redasm.set_type(obj.lpszObjectName, "str")

    if not obj.lpObjectInfo:
        return

    objinfo = redasm.get_type(obj.lpObjectInfo, "VB_OBJECT_INFO_OPTIONAL")
    if not has_optional_info(obj, objinfo):
        redasm.set_type(obj.lpObjectInfo, "VB_OBJECT_INFO")
        return

    objinfo = redasm.set_type(obj.lpObjectInfo, "VB_OBJECT_INFO_OPTIONAL")
    if not objinfo.lpControls:
        return

    ctrlinfo = redasm.set_type(objinfo.lpControls, f"VB_CONTROL_INFO[{objinfo.dwControlCount}]")

    for ctrl in ctrlinfo:
        ctrlname = redasm.set_type(ctrl.lpszName, "str")
        funcprefix = f"{objname}.{ctrlname}"

        eventinfo = redasm.set_type(ctrl.lpEventInfo, "VB_EVENT_INFO")
        redasm.set_function_as(eventinfo.lpEVENT_SINK_QueryInterface, f"{funcprefix}.QueryInterface")
        redasm.enqueue(eventinfo.lpEVENT_SINK_QueryInterface)
        redasm.set_function_as(eventinfo.lpEVENT_SINK_AddRef, f"{funcprefix}.AddRef")
        redasm.enqueue(eventinfo.lpEVENT_SINK_AddRef)
        redasm.set_function_as(eventinfo.lpEVENT_SINK_Release, f"{funcprefix}.Release")
        redasm.enqueue(eventinfo.lpEVENT_SINK_Release)


def vb_isenabled(pe):
    return pe.classifier.is_visualbasic


def vb_execute(pe):
    redasm.create_struct("GUID", VBH.GUID)
    redasm.create_struct("VB_HEADER", VBH.VB_HEADER)
    redasm.create_struct("VB_PROJECT_DATA", VBH.VB_PROJECT_DATA)
    redasm.create_struct("VB_OBJECT_TABLE", VBH.VB_OBJECT_TABLE)
    redasm.create_struct("VB_GUI_TABLE", VBH.VB_GUI_TABLE)
    redasm.create_struct("VB_OBJECT_TREE_INFO", VBH.VB_OBJECT_TREE_INFO)
    redasm.create_struct("VB_PUBLIC_OBJECT_DESCRIPTOR", VBH.VB_PUBLIC_OBJECT_DESCRIPTOR)
    redasm.create_struct("VB_OBJECT_INFO", VBH.VB_OBJECT_INFO)
    redasm.create_struct("VB_OBJECT_INFO_OPTIONAL", VBH.VB_OBJECT_INFO_OPTIONAL)
    redasm.create_struct("VB_CONTROL_INFO", VBH.VB_CONTROL_INFO)
    redasm.create_struct("VB_EVENT_INFO", VBH.VB_EVENT_INFO)

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

    vb = SimpleNamespace()
    vb.header = redasm.set_type(vbaddr, "VB_HEADER")
    vb.guitable = redasm.set_type(vb.header.lpGuiTable, "VB_GUI_TABLE")
    vb.projinfo = redasm.set_type(vb.header.lpProjectData, "VB_PROJECT_DATA")
    vb.objtable = redasm.set_type(vb.projinfo.lpObjectTable, "VB_OBJECT_TABLE")
    vb.objtreeinfo = redasm.set_type(vb.objtable.lpObjectTreeInfo, "VB_OBJECT_TREE_INFO")
    vb.pubobjdescr = redasm.set_type(vb.objtable.lpPubObjArray, f"VB_PUBLIC_OBJECT_DESCRIPTOR[{vb.objtable.wTotalObjects}]")

    redasm.set_type_as(vbaddr + vb.header.bszProjectDescription, "str", "hdrProjectDescr")
    redasm.set_type_as(vbaddr + vb.header.bszProjectExeName, "str", "hdrProjectExe")
    redasm.set_type_as(vbaddr + vb.header.bszProjectHelpFile, "str", "hdrProjectHelp")
    redasm.set_type_as(vbaddr + vb.header.bszProjectName, "str", "hdrProjectName")
    redasm.set_type_as(vb.objtable.lpszProjectName, "str", "objtableProjectName")
    redasm.set_type_as(vb.objtreeinfo.szProjectDescription, "str", "objtreeProjectDescr")
    redasm.set_type_as(vb.objtreeinfo.szProjectHelpFile, "str", "objtreeProjectHelp")

    for x in vb.pubobjdescr:
        decompile_object(vb, x)
