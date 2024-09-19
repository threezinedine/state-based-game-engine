#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt::ecs
{
    /**
     * Each entity is represented by a unique ID only, there's no actual
     *      visible object in the user's perspective.
     * If the type is changed, then must change the system trigger context
     *      of CreateEntity, DeleteEntity, and the Layer system callback.
     * The Debug click in GraphicInterface also uses this type (need to be changed).
     */
    using entity_id_t = u32;
    // constexpr entity_id_t INVALID_ENTITY_ID =
    constexpr entity_id_t INVALID_ENTITY_ID = -1;

    /**
     * Component Base contains only the data, no logic, so the struct is used
     *      rather than class. Other component should inherit from this struct.
     */
    struct ComponentBase
    {
        entity_id_t entity_id; ///< Each time a component is created, then
                               ///< it must be attached to an entity.
        b8 active = TRUE;
    };

} // namespace ntt::ecs
