# Based on: https://hex-rays.com/products/ida/support/freefiles/vb.idc
import redasm
from loaders.pe.analyzers.vb import header as VBH
from loaders.pe.analyzers.vb.controls import find_control
from loaders.pe.classifier import is_visualbasic


class VBAnalyzer:
    id = "pe_vb6"
    name = "Decompile VB5/6"
    flags = redasm.AF_SELECTED | redasm.AF_RUNONCE

    def has_optional_info(self, addr, objinfo):
        return (objinfo.lpConstants != (addr + redasm.size_of("VB_OBJECT_INFO")))

    def create_uuid(self, addr):
        if addr:
            redasm.set_type(addr, "u32[16]")

    def create_com_reg_data(self, vbheader):
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

    def create_ctrl_info(self, addr, n):
        ctrlinfo = redasm.set_type(addr, f"VB_CONTROL_INFO[{n}]")

        for ctrl in ctrlinfo:
            ctrlname = redasm.set_type(ctrl.lpszName, "str")

            if ctrl.lpGuid and ctrl.lpEventHandlerTable:
                ctrldef = find_control(redasm.set_type(ctrl.lpGuid, "GUID"))
                eventinfo = redasm.set_type(ctrl.lpEventHandlerTable, "VB_EVENT_INFO")

                if eventinfo.lpEVENT_SINK_QueryInterface:
                    redasm.set_function(eventinfo.lpEVENT_SINK_QueryInterface)

                if eventinfo.lpEVENT_SINK_AddRef:
                    redasm.set_function(eventinfo.lpEVENT_SINK_AddRef)

                if eventinfo.lpEVENT_SINK_Release:
                    redasm.set_function(eventinfo.lpEVENT_SINK_Release)

                events = redasm.set_type(ctrl.lpEventHandlerTable + redasm.size_of("VB_EVENT_INFO"),
                                         f"u32[{ctrl.wEventHandlerCount}]")

                for i, e in enumerate(events):
                    if e:
                        redasm.set_function(e)
                        redasm.set_name(e, f"{ctrlname}.{ctrldef["events"][i]}")

    def create_obj_info(self, addr):
        objinfo = redasm.get_type(addr, "VB_OBJECT_INFO")

        if self.has_optional_info(addr, objinfo):
            objinfo = redasm.set_type(addr, "VB_OBJECT_INFO_OPTIONAL")

            # if objinfo.dwObjectTypeGuids and objinfo.lpuuidObjectTypes:
            #     redasm.set_type(objinfo.lpuuidObjectTypes, f"GUID[{objinfo.dwObjectTypeGuids}]")

            # create_uuid(objinfo.lpObjectGuid)
            # create_uuid(objinfo.lpObjectGuid2)

            if objinfo.dwControlCount and objinfo.lpControls:
                self.create_ctrl_info(objinfo.lpControls, objinfo.dwControlCount)
        else:
            objinfo = redasm.set_type(addr, "VB_OBJECT_INFO")

    def create_proj_info2(self, objtable):
        projinfo2 = redasm.set_type(objtable.lpProjectInfo2, "VB_PROJECT_INFO2")

        if projinfo2.lpObjectList:
            compiledobjects = redasm.set_type(projinfo2.lpObjectList, f"u32[{objtable.wCompiledObjects}]")

            for objaddr in compiledobjects:
                redasm.set_type(objaddr, "VB_PRIVATE_OBJECT_DESCRIPTOR")

                # if privobjdescr.lpObjectInfo:
                #     create_obj_info(privobjdescr.lpObjectInfo)

    def create_obj_array(self, objtable):
        objarray = redasm.set_type(objtable.lpObjectArray, f"VB_PUBLIC_OBJECT_DESCRIPTOR[{objtable.wTotalObjects}]")

        for obj in objarray:
            if obj.lpObjectInfo:
                self.create_obj_info(obj.lpObjectInfo)

            if obj.lpszObjectName:
                redasm.set_type(obj.lpszObjectName, "str")

            if obj.lpMethodNames and obj.dwMethodCount:
                addrlist = redasm.set_type(obj.lpMethodNames, f"u32[{obj.dwMethodCount}]")

                for addr in addrlist:
                    if addr:
                        redasm.set_type(addr, "str")

    def create_obj_table(self, projinfo):
        objtable = redasm.set_type(projinfo.lpObjectTable, "VB_OBJECT_TABLE")

        if objtable.lpProjectInfo2:
            self.create_proj_info2(objtable)

        if objtable.lpObjectArray:
            self.create_obj_array(objtable)

    def create_proj_info(self, vbheader):
        projinfo = redasm.set_type(vbheader.lpProjectData, "VB_PROJECT_INFO")

        if projinfo.lpObjectTable:
            self.create_obj_table(projinfo)

    @staticmethod
    def is_enabled():
        plugin = redasm.get_loaderplugin()

        if plugin.id == "pe":
            pe = redasm.get_loader()
            return is_visualbasic(pe.classifier.value)

        return False

    def execute(self):
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

        ep = redasm.get_entries()
        if len(ep) != 1:
            return

        func = redasm.rdil.create_function(ep[0])
        if (len(func) < 2 or (func[0].expr.op != redasm.rdil.PUSH and
                              func[1].expr.op != redasm.rdil.CALL)):
            return

        vbaddr = func[0].expr.u.addr
        signature = redasm.get_type(vbaddr, "char[4]")

        if signature != "VB5!":
            return

        vbheader = redasm.set_type(vbaddr, "VB_HEADER")
        redasm.set_type(vbaddr + vbheader.bSZProjectDescription, "str")
        redasm.set_type(vbaddr + vbheader.bSZProjectExeName, "str")
        redasm.set_type(vbaddr + vbheader.bSZProjectHelpFile, "str")
        redasm.set_type(vbaddr + vbheader.bSZProjectName, "str")

        if vbheader.lpSubMain:
            redasm.set_function(vbheader.lpSubMain)
            redasm.set_name(vbheader.lpSubMain, "SubMain")

        if vbheader.lpComRegisterData:
            self.create_com_reg_data(vbheader)

        if vbheader.lpProjectData:
            self.create_proj_info(vbheader)
