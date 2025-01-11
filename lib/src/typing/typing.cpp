#include "typing.h"
#include "../error.h"

namespace redasm::typing {

const TypeDef* Types::declare(const std::string& name, const Struct& arg) {
    if(name.empty()) except("Struct name is empty");
    if(arg.empty()) except("Struct cannot be empty");

    TypeDef type{};
    type.name = name;

    if(this->m_registered.contains(type.get_id()))
        except("Struct '{}' already exists", name);

    for(const auto& [tname, name] : arg) {
        ParsedType pt;
        type.size += this->size_of(tname, &pt);

        if(pt.tdef->is_var()) {
            except("Type '{}' size is variable and is not supported in structs",
                   tname);
        }

        type.dict.emplace_back(RDType{.id = pt.tdef->get_id(), .n = pt.n},
                               name);
    }

    auto it = this->m_registered.try_emplace(type.get_id(), type);
    assume(it.second);
    return &it.first->second;
}

} // namespace redasm::typing
