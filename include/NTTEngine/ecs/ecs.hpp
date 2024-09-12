#pragma once
#include <NTTEngine/defines.hpp>
#include <typeindex>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>

/**
 * Manage all the entity inside the game. This module has 3 main components:
 *      - Entity: The object in the world and is defined by a unique ID and
 *          contains a list of components.
 *      - Component: The group of generic data that can be attached to any
 *          entity. The component contains the data only, no logic.
 *      - System: The Updatable system will automatically received the needed
 *          component from the entity and the system can update that component.
 *          The system has the logic only, no data.
 */
namespace ntt::ecs
{
    /**
     * Each entity is represented by a unique ID only, there's no actual
     *      visible object in the user's perspective.
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

    /**
     * Start the ECS system. This function must be called before any other
     */
    void ECSInit();

    /**
     * Store all lifetime functionalitiy of a system.
     */
    struct System
    {
        std::function<void(entity_id_t)> init;
        std::function<void(f32, entity_id_t)> update;
        std::function<void(entity_id_t)> shutdown;

        System()
            : init([](entity_id_t) {}),
              update([](f32, entity_id_t) {}),
              shutdown([](entity_id_t) {}) {}

        System(std::function<void(f32, entity_id_t)> update)
            : init([](entity_id_t) {}),
              update(update),
              shutdown([](entity_id_t) {}) {}

        System(std::function<void(entity_id_t)> init,
               std::function<void(f32, entity_id_t)> update,
               std::function<void(entity_id_t)> shutdown)
            : init(init), update(update), shutdown(shutdown) {}

        System(const System &other)
            : init(other.init), update(other.update), shutdown(other.shutdown) {}
    };

    /**
     * Add new system to the ECS, the order of adding the system is the order
     *      of the system will be updated.
     *
     * @param system The system to be added to the ECS
     * @param componentTypes The list of component types that the system needs
     */
    void ECSRegister(String name, System system, List<std::type_index> componentTypes);

    /**
     * Get all list of entities which are attached to the system.
     *
     * @param name The name of the system which
     *      is registered in the ECS system
     *
     */
    List<entity_id_t> ECSGetEntitiesWithSystem(String name);

    /**
     * When a new entity is created with attached components, the entity will
     *      be passed to the system which needs the components.
     *
     * @param components The list of components to be attached to the entity
     *
     * @return The ID of the created entity, if there's an error, then return
     *      INVALID_ENTITY_ID
     */
    entity_id_t ECSCreateEntity(Dictionary<std::type_index, Ref<ComponentBase>> components);

    /**
     * Query the component based on the entity ID and the type of the component.
     * Each entity has only 1 instance of each type of component.
     *
     * @param id The ID of the entity
     * @param type The type of the component
     *
     * @return The component which is attached to the entity, if the id
     *      is invalid or not found or the component type is not included
     *      then return nullptr
     */
    Ref<ComponentBase> ECSGetComponent(entity_id_t id, std::type_index type);

    /**
     * Changing the state of the component, if the component is not active,
     *      then it does not affect the supported systems.
     *
     * If the id or the component type is not found, then nothing will be changed
     *      the warning will be logged.
     *
     * @param id The ID of the entity
     * @param type The type of the component
     * @param active The state of the component
     */
    void ECSSetComponentActive(entity_id_t id, std::type_index type, b8 active = TRUE);

    /**
     * Delete the entity and all the components attached to the entity.
     *
     * @param id The ID of the entity to be deleted
     *      if the id is invalid, then nothing will be deleted
     *      and the warning will be logged
     */
    void ECSDeleteEntity(entity_id_t id);

    /**
     * Update the ECS system. This function must be called every frame for
     *      having updated for each registered system.
     */
    void ECSUpdate(f32 delta);

    /**
     * Destroy the ECS system. This function must be called when the ECS system
     *      is no longer needed.
     */
    void ECSShutdown();
} // namespace ntt::ecs

#define ECS_GET_COMPONENT(id, type) std::static_pointer_cast<type>(ECSGetComponent(id, typeid(type)))
#define ECS_CREATE_COMPONENT(type, ...) {typeid(type), std::make_shared<type>(__VA_ARGS__)}