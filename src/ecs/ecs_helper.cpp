#include <NTTEngine/ecs/ecs_helper.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/TextureComponent.hpp>
#include <NTTEngine/physics/Mass.hpp>
#include <NTTEngine/renderer/Sprite.hpp>

namespace ntt
{
    using namespace memory;
    using namespace ecs;
    using namespace renderer;
    using namespace physics;

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
