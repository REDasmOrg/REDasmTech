#include "signature.h"
#include "../plugins/modulemanager.h"
#include "../utils/utils.h"
#include <filesystem>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/schema.h>
#include <redasm/ct.h>
#include <spdlog/spdlog.h>

namespace redasm::signature {

namespace fs = std::filesystem;

namespace {

constexpr std::string_view EXT = ".json";
constexpr std::string_view SIGNATURES_ROOT = "signatures";

constexpr std::string_view SCHEMA_JSON = R"({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": {
        "signatures": {
            "type": "object",
            "additionalProperties": false,
            "required": [
                "cc",
                "ret",
                "args"
            ],
            "properties": {
                "cc": "string",
                "ret": "int",
                "args": {
                    "type": "array",
                    "arguments": {
                        "required": [
                            "name",
                            "type"
                        ],
                        "properties": {
                            "name": "string",
                            "type": "string"
                        }
                    }
                }
            }
        }
    }
})";

rapidjson::Document schema;

rapidjson::Document parse(const std::string& sigfile) {
    if(schema.IsNull()) {
        schema.Parse(signature::SCHEMA_JSON.data()); // NOLINT
        ct_assume(!schema.HasParseError());
    }

    std::ifstream ifs{sigfile};
    ct_assume(ifs.is_open());
    rapidjson::IStreamWrapper ifsw{ifs};

    rapidjson::Document doc;
    doc.ParseStream(ifsw);

    if(doc.HasParseError()) {
        spdlog::error("Cannot load signature @ '{}': ({} at {})", sigfile,
                      rapidjson::GetParseError_En(doc.GetParseError()),
                      doc.GetErrorOffset());

        return {};
    }

    rapidjson::SchemaDocument sdoc{schema};
    rapidjson::SchemaValidator validator{sdoc};

    if(!doc.Accept(validator)) {
        rapidjson::StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);

        spdlog::error("Schema validator error for: '{}'\n{}\n{}", sigfile,
                      sb.GetString(), validator.GetInvalidSchemaKeyword());

        return {};
    }

    spdlog::info("Signature '{}' loaded", sigfile);
    return doc;
}

rapidjson::Document load(std::string_view name) {
    const mm::SearchPaths& searchpaths = mm::get_searchpaths();

    for(fs::path sp : searchpaths) {
        sp /= signature::SIGNATURES_ROOT;

        if(fs::directory_entry e{sp}; e.is_directory()) {
            fs::path sigfile = (e.path() / name);
            sigfile.replace_extension(signature::EXT);
            if(!fs::is_regular_file(sigfile)) continue;
            return signature::parse(sigfile);
        }
    }

    return {};
}

} // namespace

SignatureManager::SignatureManager() {
    hmap_init(&this->signatures, str_hash_cstr);
}

SignatureManager::~SignatureManager() {
    RDSignature *sig, *temp;
    hmap_foreach_safe(sig, temp, &this->signatures, RDSignature, hnode) {
        delete[] sig->cc;
        delete[] sig->ret;
        delete[] sig->name;

        RDSignatureArgsItem* it;
        slice_foreach(it, &sig->args) {
            delete[] it->type;
            delete[] it->name;
        }

        slice_destroy(&sig->args);
        delete sig;
    }
}

bool SignatureManager::add(std::string_view sigfile) {
    rapidjson::Document doc = signature::load(sigfile);
    if(!doc.IsObject()) return false;

    for(const auto& [k, v] : doc.GetObject()) {
        auto* sig = new RDSignature{};
        sig->name = utils::copy_str(k.GetString());
        sig->cc = utils::copy_str(v["cc"].GetString());
        sig->ret = utils::copy_str(v["ret"].GetString());
        slice_init(&sig->args, nullptr, nullptr);

        auto args = v["args"].GetArray();
        slice_reserve(&sig->args, args.Size());

        for(const auto& arg : args) {
            slice_push(&sig->args,
                       {
                           .type = utils::copy_str(arg["type"].GetString()),
                           .name = utils::copy_str(arg["name"].GetString()),
                       });
        }

        hmap_set(&this->signatures, &sig->hnode, sig->name);
    }

    return true;
}

bool SignatureManager::empty() const { return hmap_empty(&this->signatures); }

const RDSignature* SignatureManager::find(std::string_view n) const {
    const RDSignature* sig;
    hmap_get(sig, &this->signatures, n.data(), RDSignature, hnode,
             n == sig->name);
    return sig;
}

} // namespace redasm::signature
