#pragma once
#include <typeindex>
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>
#include "component_base.hpp"

namespace ntt::ecs
{
    struct EntityInfo
    {
        Dictionary<std::type_index, Ref<ComponentBase>> components;
        b8 active = TRUE;

        EntityInfo(Dictionary<std::type_index, Ref<ComponentBase>> components)
            : components(components), active(TRUE)
        {
        }
    };

}