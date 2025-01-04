#include "dex.h"
#include "demangler.h"
#include <libdex/DexClass.h>
#include <libdex/DexFile.h>
#include <list>
#include <string>
#include <vector>

namespace dex {

namespace {

std::string demangle(std::string s) {
    if(s.front() == '[') return dex::demangle(s.substr(1)) + "[]";
    if(s == "V") return "void";
    if(s == "Z") return "boolean";
    if(s == "B") return "byte";
    if(s == "S") return "short";
    if(s == "C") return "char";
    if(s == "I") return "int";
    if(s == "J") return "long";
    if(s == "F") return "float";
    if(s == "D") return "double";

    if(s.front() == 'L') s.erase(s.begin());
    if(s.back() == ';') s.pop_back();
    std::replace(s.begin(), s.end(), '/', '.');
    // std::replace(s.begin(), s.end(), '$', '.');
    return s;
}

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

bool filter_classes(const DexFile* df) {
    std::vector<const DexClassDef*> classdefs;
    std::vector<RDUIOptions> options;
    std::list<std::string> classtypes;
    for(u32 i = 0; i < df->pHeader->classDefsSize; i++) {
        const DexClassDef* classdef = dexGetClassDef(df, i);
        if(!classdef) continue;
        const char* pclassdescr = dexGetClassDescriptor(df, classdef);
        if(!pclassdescr) continue;

        bool precheck = true;
        std::string packagename = demangler::get_packagename(pclassdescr);

        if(!packagename.find("android.") ||
           !packagename.find("com.google.")) // Apply prefiltering
            precheck = false;

        // Cache Strings
        classtypes.push_back(demangler::get_objectname(pclassdescr));
        options.push_back({classtypes.back().c_str(), precheck});
        classdefs.push_back(classdef);
    }

    if(!rdui_getchecked("Class Loader",
                        "Select one or more objects from the list below",
                        options.data(), options.size()))
        return false;

    for(usize i = 0; i < classdefs.size(); i++)
        dex::load_class(df, classdefs[i], options[i].selected);

    return true;
}

} // namespace

bool init(RDLoader*) {
    const u8* data = nullptr;
    usize n = rd_getfile(&data);
    if(!n) return false;

    DexFile* df = dexFileParse(data, n, 0);
    if(!df) return false;

    rd_map(0, n);
    rd_mapsegment_n("CODE", df->pHeader->dataOff, df->pHeader->dataSize,
                    df->pHeader->dataOff, df->pHeader->dataSize, SEG_HASCODE);

    bool ok = dex::filter_classes(df);
    if(ok) rd_setprocessor("dalvik");
    dexFileFree(df);
    return ok;
}

} // namespace dex
