#include <NTTEngine/ecs/ecs_helper.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/TextureComponent.hpp>
#include <NTTEngine/physics/Mass.hpp>
#include <NTTEngine/renderer/Sprite.hpp>
#include <NTTEngine/application/script_system/script_component.hpp>
#include <NTTEngine/renderer/Hovering.hpp>
#include <NTTEngine/application/script_system/state_component.hpp>

namespace ntt
{
    Dictionary<std::type_index, Ref<ComponentBase>> ECS_From(const JSON &json)
    {
        Dictionary<std::type_index, Ref<ComponentBase>> components;

        if (json.Contains<JSON>("Geometry"))
        {
            Ref<Geometry> geometry = CreateRef<Geometry>();
            geometry->FromJSON(json.Get<JSON>("Geometry"));
            components[typeid(Geometry)] = geometry;
        }

        if (json.Contains<JSON>("TextureComponent"))
        {
            Ref<TextureComponent> textureComponent = CreateRef<TextureComponent>();
            textureComponent->FromJSON(json.Get<JSON>("TextureComponent"));
            components[typeid(TextureComponent)] = textureComponent;
        }

        if (json.Contains<JSON>("Mass"))
        {
            Ref<Mass> mass = CreateRef<Mass>();
            mass->FromJSON(json.Get<JSON>("Mass"));
            components[typeid(Mass)] = mass;
        }

        if (json.Contains<JSON>("Sprite"))
        {
            Ref<Sprite> sprite = CreateRef<Sprite>();
            sprite->FromJSON(json.Get<JSON>("Sprite"));
            components[typeid(Sprite)] = sprite;
        }

        if (json.Contains<JSON>("NativeScriptComponent"))
        {
            Ref<NativeScriptComponent> nativeScriptComponent = CreateRef<NativeScriptComponent>();
            nativeScriptComponent->FromJSON(json.Get<JSON>("NativeScriptComponent"));
            components[typeid(NativeScriptComponent)] = nativeScriptComponent;
        }

        if (json.Contains<JSON>("Hovering"))
        {
            Ref<Hovering> hovering = CreateRef<Hovering>();
            hovering->FromJSON(json.Get<JSON>("Hovering"));
            components[typeid(Hovering)] = hovering;
        }

        if (json.Contains<JSON>("StateComponent"))
        {
            Ref<StateComponent> stateComponent = CreateRef<StateComponent>();
            stateComponent->FromJSON(json.Get<JSON>("StateComponent"));
            components[typeid(StateComponent)] = stateComponent;
        }

        return components;
    }

    JSON ECS_ToJSON(const Dictionary<std::type_index, Ref<ComponentBase>> &components)
    {
        JSON json("{}");

        for (auto &component : components)
        {
            json.Set(component.second->GetName(), component.second->ToJSON());
        }

        return json;
    }
} // namespace ntt
