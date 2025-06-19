#include "signature.h"
#include "../plugins/modulemanager.h"
#include <filesystem>
#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

namespace redasm::signature {

constexpr std::string_view EXT = ".json";
constexpr std::string_view SIGNATURES_ROOT = "signatures";

namespace fs = std::filesystem;

bool SignatureManager::load(std::string_view name) {
    const mm::SearchPaths& searchpaths = mm::get_searchpaths();

    for(fs::path sp : searchpaths) {
        sp /= signature::SIGNATURES_ROOT;
        spdlog::critical("## {}", sp.string());

        if(fs::directory_entry e{sp}; e.is_directory()) {
            fs::path sigfile = (e.path() / name);
            sigfile.replace_extension(signature::EXT);
            spdlog::critical("~~ {}", sigfile.string());

            if(!fs::is_regular_file(sigfile)) continue;
            spdlog::critical(">> {}", sigfile.string());
        }
    }

    return false;
}

void compile() {}

} // namespace redasm::signature
