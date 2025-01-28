#include "dex.h"
#include "demangler.h"
#include <libdex/DexClass.h>
#include <list>
#include <string>
#include <vector>

namespace dex {

namespace {

void load_method(const DexFile* df, const DexMethod& dexmethod, bool filter) {
    if(!dexmethod.codeOff) return;

    const DexCode* dexcode = dexGetCode(df, &dexmethod);
    const DexMethodId* methodid = dexGetMethodId(df, dexmethod.methodIdx);
    std::string methodname = dexStringById(df, methodid->nameIdx);
    std::string classname =
        demangler::get_objectname(dexStringByTypeIdx(df, methodid->classIdx));
    std::string fullname = classname + "." + methodname;

    RDAddress addr = reinterpret_cast<const u8*>(dexcode->insns) - df->baseAddr;

    if(filter) {
        rd_settypename(addr, "u16", nullptr);
        rd_setname_ex(addr, fullname.c_str(), SN_IMPORT);
    }
    else
        rd_setentry(addr, fullname.c_str());
}

void load_class(const DexFile* df, const DexClassDef* classdef, bool filter) {
    const dex_u1* encodeddata = dexGetClassData(df, classdef);
    DexClassData* classdata = dexReadAndVerifyClassData(&encodeddata, nullptr);

    if(classdata) {
        for(dex_u4 i = 0; i < classdata->header.directMethodsSize; i++)
            dex::load_method(df, classdata->directMethods[i], filter);

        for(dex_u4 i = 0; i < classdata->header.virtualMethodsSize; i++)
            dex::load_method(df, classdata->virtualMethods[i], filter);
    }

    std::free(classdata);
}

bool filter_classes(const DexLoader* self) {
    std::vector<const DexClassDef*> classdefs;
    std::vector<RDUIOptions> options;
    std::list<std::string> classtypes;

    for(u32 i = 0; i < self->dexfile->pHeader->classDefsSize; i++) {
        const DexClassDef* classdef = dexGetClassDef(self->dexfile, i);
        if(!classdef) continue;

        const char* pclassdescr =
            dexGetClassDescriptor(self->dexfile, classdef);
        if(!pclassdescr) continue;

        bool precheck = true;
        std::string packagename = demangler::get_packagename(pclassdescr);

        if(!packagename.find("android.") ||
           !packagename.find("com.google.")) // Apply prefiltering
            precheck = false;

        // Cache Strings
        classtypes.push_back(demangler::get_objectname(pclassdescr));

        options.push_back({
            .text = classtypes.back().c_str(),
            .selected = precheck,
        });

        classdefs.push_back(classdef);
    }

    if(!rdui_getchecked("Class Loader",
                        "Select one or more objects from the list below",
                        options.data(), options.size()))
        return false;

    for(usize i = 0; i < classdefs.size(); i++)
        dex::load_class(self->dexfile, classdefs[i], options[i].selected);

    return true;
}

bool accept(const RDLoaderPlugin*, const RDLoaderRequest* req) {
    const u8* data = nullptr;
    usize n = rdbuffer_getdata(req->file, &data);
    DexFile* df = dexFileParse(data, n, 0);
    bool ok = df != nullptr;
    dexFileFree(df);
    return ok;
}

bool load(RDLoader* l, RDBuffer*) {
    auto* self = reinterpret_cast<DexLoader*>(l);
    const u8* data = nullptr;
    usize n = rd_getfile(&data);
    self->dexfile = dexFileParse(data, n, 0);

    rd_map(0, n);
    rd_mapsegment_n("CODE", self->dexfile->pHeader->dataOff,
                    self->dexfile->pHeader->dataSize,
                    self->dexfile->pHeader->dataOff,
                    self->dexfile->pHeader->dataSize, SP_RWX);

    return dex::filter_classes(self);
}

} // namespace

RDLoaderPlugin loader = {
    .level = REDASM_API_LEVEL,
    .id = "dex",
    .name = "Dalvik Executable",

    .create =
        [](const RDLoaderPlugin*) {
            return reinterpret_cast<RDLoader*>(new DexLoader{});
        },

    .destroy =
        [](RDLoader* l) {
            auto* self = reinterpret_cast<DexLoader*>(l);
            if(self->dexfile) dexFileFree(self->dexfile);
            delete self;
        },

    .flags = LF_NOMERGECODE | LF_NOAUTORENAME,
    .accept = dex::accept,
    .load = dex::load,
    .get_processor = [](RDLoader*) { return "dalvik"; },
};

} // namespace dex
