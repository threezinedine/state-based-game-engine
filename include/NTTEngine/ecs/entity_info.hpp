#pragma once
#include <typeindex>
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>
#include "component_base.hpp"
#include <NTTEngine/core/object.hpp>
#include <functional>

namespace ntt::ecs
{
    struct EntityInfo : public Object
    {
        Dictionary<std::type_index, Ref<ComponentBase>> components;
        String name;

        void FromJSON(const JSON &entity);
        JSON ToJSON() const;

        void OnEditorUpdate(std::function<void()> onChanged = nullptr, void *data = nullptr);
    };
}