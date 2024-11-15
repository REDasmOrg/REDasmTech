# Based on: https://hex-rays.com/products/ida/support/freefiles/vb.idc
import redasm
from . import header as VBH
from .controls import find_control


# def decompile_object(vb, obj):
#     objname = redasm.set_type(obj.lpszObjectName, "str")
#
#     if not obj.lpObjectInfo:
#         return
#
#     objinfo = redasm.get_type(obj.lpObjectInfo, "VB_OBJECT_INFO_OPTIONAL")
#     if not has_optional_info(obj, objinfo):
#         redasm.set_type(obj.lpObjectInfo, "VB_OBJECT_INFO")
#         return
#
#     objinfo = redasm.set_type(obj.lpObjectInfo, "VB_OBJECT_INFO_OPTIONAL")
#     if not objinfo.lpControls:
#         return
#
#     ctrlinfo = redasm.set_type(objinfo.lpControls, f"VB_CONTROL_INFO[{objinfo.dwControlCount}]")
#
#     for ctrl in ctrlinfo:
#         ctrlname = redasm.set_type(ctrl.lpszName, "str")
#         funcprefix = f"{objname}.{ctrlname}"
#
#         eventinfo = redasm.set_type(ctrl.lpEventInfo, "VB_EVENT_INFO")
#         print(eventinfo)
#         redasm.set_function_as(eventinfo.lpEVENT_SINK_QueryInterface, f"{funcprefix}.QueryInterface")
#         redasm.enqueue(eventinfo.lpEVENT_SINK_QueryInterface)
#         redasm.set_function_as(eventinfo.lpEVENT_SINK_AddRef, f"{funcprefix}.AddRef")
#         redasm.enqueue(eventinfo.lpEVENT_SINK_AddRef)
#         redasm.set_function_as(eventinfo.lpEVENT_SINK_Release, f"{funcprefix}.Release")
#         redasm.enqueue(eventinfo.lpEVENT_SINK_Release)
#
#         c = find_component(redasm.set_type(ctrl.lpGuid, "GUID"))
#
#         if c:
#             print(c["events"])
#             events = redasm.get_type(ctrl.lpEventInfo + redasm.size_of("VB_EVENT_INFO"), f"u32[{len(c["events"])}]")
#             print(events)
#
#             for i, e in enumerate(c["events"]):
#                 if events[i]:
#                     redasm.set_function_as(events[i], f"{funcprefix}.{e}")
#                     redasm.enqueue(events[i])


def has_optional_info(addr, objinfo):
    return (objinfo.lpConstants != (addr + redasm.size_of("VB_OBJECT_INFO")))


def create_uuid(addr):
    if addr:
        redasm.set_type(addr, "u32[16]")


def create_com_reg_data(vbheader):
    comregdata = redasm.set_type(vbheader.lpComRegisterData,
                                 "COM_REGISTRATION_DATA")

    if comregdata.bSZProjectName:
        redasm.set_type(vbheader.lpComRegisterData + comregdata.bSZProjectName,
                        "str")

    if comregdata.bSZHelpDirectory:
        redasm.set_type(vbheader.lpComRegisterData + comregdata.bSZHelpDirectory,
                        "str")

    if comregdata.bSZProjectDescription:
        redasm.set_type(vbheader.lpComRegisterData + comregdata.bSZProjectDescription,
                        "str")

    if comregdata.bRegInfo:
        redasm.set_type(vbheader.lpComRegisterData + comregdata.bRegInfo,
                        "COM_REGISTRATION_INFO")


def create_ctrl_info(addr, n):
    ctrlinfo = redasm.set_type(addr, f"VB_CONTROL_INFO[{n}]")

    for ctrl in ctrlinfo:
        ctrlname = redasm.set_type(ctrl.lpszName, "str")

        if ctrl.lpGuid and ctrl.lpEventHandlerTable:
            ctrldef = find_control(redasm.set_type(ctrl.lpGuid, "GUID"))
            eventinfo = redasm.set_type(ctrl.lpEventHandlerTable, "VB_EVENT_INFO")

            if eventinfo.lpEVENT_SINK_QueryInterface:
                redasm.set_function(eventinfo.lpEVENT_SINK_QueryInterface)
                redasm.enqueue(eventinfo.lpEVENT_SINK_QueryInterface)

            if eventinfo.lpEVENT_SINK_AddRef:
                redasm.set_function(eventinfo.lpEVENT_SINK_AddRef)
                redasm.enqueue(eventinfo.lpEVENT_SINK_AddRef)

            if eventinfo.lpEVENT_SINK_Release:
                redasm.set_function(eventinfo.lpEVENT_SINK_Release)
                redasm.enqueue(eventinfo.lpEVENT_SINK_Release)

            print(ctrlname, ctrldef["name"], hex(ctrl.lpEventHandlerTable))

            events = redasm.set_type(ctrl.lpEventHandlerTable + redasm.size_of("VB_EVENT_INFO"),
                                     f"u32*[{ctrl.wEventHandlerCount}]")

            for e in events:
                if e:
                    redasm.set_function(e)
                    redasm.enqueue(e)


def create_obj_info(addr):
    objinfo = redasm.get_type(addr, "VB_OBJECT_INFO")

    if has_optional_info(addr, objinfo):
        objinfo = redasm.set_type(addr, "VB_OBJECT_INFO_OPTIONAL")

        # if objinfo.dwObjectTypeGuids and objinfo.lpuuidObjectTypes:
        #     redasm.set_type(objinfo.lpuuidObjectTypes, f"GUID[{objinfo.dwObjectTypeGuids}]")

        # create_uuid(objinfo.lpObjectGuid)
        # create_uuid(objinfo.lpObjectGuid2)

        if objinfo.dwControlCount and objinfo.lpControls:
            create_ctrl_info(objinfo.lpControls, objinfo.dwControlCount)
    else:
        objinfo = redasm.set_type(addr, "VB_OBJECT_INFO")


def create_proj_info2(objtable):
    projinfo2 = redasm.set_type(objtable.lpProjectInfo2, "VB_PROJECT_INFO2")

    if projinfo2.lpObjectList:
        compiledobjects = redasm.set_type(projinfo2.lpObjectList, f"u32*[{objtable.wCompiledObjects}]")

        for i, objaddr in enumerate(compiledobjects):
            privobjdescr = redasm.set_type(objaddr, "VB_PRIVATE_OBJECT_DESCRIPTOR")

            if privobjdescr.lpObjectInfo:
                create_obj_info(privobjdescr.lpObjectInfo)


def create_obj_array(objtable):
    objarray = redasm.set_type(objtable.lpObjectArray, f"VB_PUBLIC_OBJECT_DESCRIPTOR[{objtable.wTotalObjects}]")

    for obj in objarray:
        if obj.lpObjectInfo:
            create_obj_info(obj.lpObjectInfo)

        if obj.lpszObjectName:
            redasm.set_type(obj.lpszObjectName, "str")

        if obj.lpMethodNames and obj.dwMethodCount:
            addrlist = redasm.set_type(obj.lpMethodNames, f"u32*[{obj.dwMethodCount}]")

            for addr in addrlist:
                if addr:
                    redasm.set_type(addr, "str")


def create_obj_table(projinfo):
    objtable = redasm.set_type(projinfo.lpObjectTable, "VB_OBJECT_TABLE")

    if objtable.lpProjectInfo2:
        create_proj_info2(objtable)

    if objtable.lpObjectArray:
        create_obj_array(objtable)


def create_proj_info(vbheader):
    projinfo = redasm.set_type(vbheader.lpProjectData, "VB_PROJECT_INFO")

    if projinfo.lpObjectTable:
        create_obj_table(projinfo)


def vb_isenabled(pe):
    return pe.classifier.is_visualbasic


def vb_execute(pe):
    redasm.create_struct("GUID", VBH.GUID)
    redasm.create_struct("VB_HEADER", VBH.VB_HEADER)
    redasm.create_struct("COM_REGISTRATION_DATA", VBH.COM_REGISTRATION_DATA)
    redasm.create_struct("COM_REGISTRATION_INFO", VBH.COM_REGISTRATION_INFO)
    redasm.create_struct("VB_PROJECT_INFO", VBH.VB_PROJECT_INFO)
    redasm.create_struct("VB_PROJECT_INFO2", VBH.VB_PROJECT_INFO2)
    redasm.create_struct("VB_OBJECT_TABLE", VBH.VB_OBJECT_TABLE)
    redasm.create_struct("VB_OBJECT_INFO", VBH.VB_OBJECT_INFO)
    redasm.create_struct("VB_OBJECT_INFO_OPTIONAL", VBH.VB_OBJECT_INFO_OPTIONAL)
    redasm.create_struct("VB_PRIVATE_OBJECT_DESCRIPTOR", VBH.VB_PRIVATE_OBJECT_DESCRIPTOR)
    redasm.create_struct("VB_PUBLIC_OBJECT_DESCRIPTOR", VBH.VB_PUBLIC_OBJECT_DESCRIPTOR)
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

    vbheader = redasm.set_type_as(vbaddr, "VB_HEADER", "vbheader")
    redasm.set_type(vbaddr + vbheader.bSZProjectDescription, "str")
    redasm.set_type(vbaddr + vbheader.bSZProjectExeName, "str")
    redasm.set_type(vbaddr + vbheader.bSZProjectHelpFile, "str")
    redasm.set_type(vbaddr + vbheader.bSZProjectName, "str")

    if vbheader.lpSubMain:
        redasm.set_function_as(vbheader.lpSubMain, "SubMain")
        redasm.enqueue(vbheader.lpSubMain)

    if vbheader.lpComRegisterData:
        create_com_reg_data(vbheader)

    if vbheader.lpProjectData:
        create_proj_info(vbheader)

    # vb.guitable = redasm.set_type(vb.header.lpGuiTable, "VB_GUI_TABLE")
    # vb.objtreeinfo = redasm.set_type(vb.objtable.lpObjectTreeInfo, "VB_OBJECT_TREE_INFO")
    # vb.pubobjdescr = redasm.set_type(vb.objtable.lpPubObjArray, f"VB_PUBLIC_OBJECT_DESCRIPTOR[{vb.objtable.wTotalObjects}]")

    # redasm.set_type_as(vb.objtable.lpszProjectName, "str", "objtableProjectName")
    # redasm.set_type_as(vb.objtreeinfo.szProjectDescription, "str", "objtreeProjectDescr")
    # redasm.set_type_as(vb.objtreeinfo.szProjectHelpFile, "str", "objtreeProjectHelp")
    #
    # for x in vb.pubobjdescr:
    #     decompile_object(vb, x)
