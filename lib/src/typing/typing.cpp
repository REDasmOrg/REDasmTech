#include "typing.h"
#include "../error.h"

namespace redasm::typing {

const TypeDef* Types::declare(const std::string& name,
                              const RDStructField* fields) {
    if(name.empty()) except("Struct name is empty");

    if(!fields || !fields->name || !fields->type)
        except("Struct cannot be empty");

    TypeDef type{};
    type.name = name;

    if(this->m_registered.contains(type.get_id()))
        except("Struct '{}' already exists", name);

    while(fields->name && fields->type) {
        ParsedType pt;
        type.size += this->size_of(fields->type, &pt);

        if(pt.tdef->is_var()) {
            except("Type '{}' size is variable and is not supported in structs",
                   fields->type);
        }

        type.dict.emplace_back(RDType{.id = pt.tdef->get_id(), .n = pt.n},
                               fields->name);

        fields++;
    }

    auto it = this->m_registered.try_emplace(type.get_id(), type);
    assume(it.second);
    return &it.first->second;
}

} // namespace redasm::typing
