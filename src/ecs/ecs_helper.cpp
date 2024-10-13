#include <NTTEngine/ecs/ecs_helper.hpp>
#include <NTTEngine/renderer/Geometry.hpp>

namespace ntt
{
    using namespace memory;
    using namespace ecs;
    using namespace renderer;

    Dictionary<std::type_index, Ref<ComponentBase>> ECS_From(const JSON &json)
    {
        Dictionary<std::type_index, Ref<ComponentBase>> components;

        if (json.Contains<JSON>("Geometry"))
        {
            Ref<Geometry> geometry = CreateRef<Geometry>();
            geometry->FromJSON(json.Get<JSON>("Geometry"));
            components[typeid(Geometry)] = geometry;
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
