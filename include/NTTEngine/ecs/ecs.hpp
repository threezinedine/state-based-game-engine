#pragma once
#include <NTTEngine/defines.hpp>
#include <typeindex>
#include "component_base.hpp"
#include "data_com.hpp"
#include "entity_info.hpp"
#include "system.hpp"
#include "layer_types.hpp"

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
namespace ntt
{
    /**
     * Start the ECS system. This function must be called before any other
     */
    void ECSInit();

    /**
     * Add new system to the ECS, the order of adding the system is the order
     *      of the system will be updated.
     *
     * @param name The name of the system, use for debugging only
     * @param system The system to be added to the ECS
     * @param componentTypes The list of component types that the system needs
     * @param runInDebug If the system is running in the debug mode or not
     */
    void ECSRegister(String name,
                     Ref<System> system,
                     List<std::type_index> componentTypes,
                     b8 alwayUpdate = FALSE);

    /**
     * Change the state of the system, if the system is not active, then
     *      the system will not be updated.
     *
     * If the system is not found, then nothing will be changed and the warning
     *     will be logged.
     *
     * @param name The name of the system which is registered in the ECS system
     * @param active The state of the system
     */
    void ECSChangeSystemState(String name, b8 active = TRUE);

    /**
     * The beginning of a new layer inside the system, the new layer will
     *      be on top of the previous layers, all entities which are created
     *      after this new layer will be attached to this layer.
     * User can begin a layer multiple times, but if the layer is created, then
     *      the old layer will added new entities to the existed layer.
     *
     * @example:
     * ```C++
     *      ECSBeginLayer(GAME_LAYER);
     *
     *      ECS_CREATE_ENTITY("Player", {
     *          {typeid(TransformComponent), CREATE_REF(TransformComponent, 0.0f, 0.0f, 0.0f)},
     *          {typeid(SpriteComponent), CREATE_REF(SpriteComponent, "player.png")}
     *      }); // now the "Player" entity is attached to the GAME_LAYER and the
     *          // game layer will be the lowest layer in the system
     *
     *      ECSBeginLayer(UI_LAYER);
     *      ECS_CREATE_ENTITY("Button", {
     *          {typeid(TransformComponent), CREATE_REF(TransformComponent, 0.0f, 0.0f, 0.0f)},
     *          {typeid(SpriteComponent), CREATE_REF(SpriteComponent, "button.png")}
     *      }); // now the "Button" entity is attached to the UI_LAYER and the
     *          // UI layer will be on top of the GAME_LAYER
     *
     *      ECSBeginLayer(GAME_LAYER);
     *      ECS_CREATE_ENTITY("Enemy", {
     *          {typeid(TransformComponent), CREATE_REF(TransformComponent, 0.0f, 0.0f, 0.0f)},
     *          {typeid(SpriteComponent), CREATE_REF(SpriteComponent, "enemy.png")}
     *      }); // now the "Enemy" entity is attached to the GAME_LAYER and the
     *          // game layer is still the lowest layer in the system
     * ```
     */
    void ECSBeginLayer(layer_t layer);

    /**
     * Staring drawing the layers on the screen
     *      if the layer is already visible then
     *      the layer will be drawn on the screen
     * Other layers will be hidden
     */
    void ECSLayerMakeVisible(layer_t layer);

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
     * @param name The name of the entity (use for debugging only)
     * @param components The list of components to be attached to the entity
     *
     * @return The ID of the created entity, if there's an error, then return
     *      INVALID_ENTITY_ID
     */
    entity_id_t ECSCreateEntity(
        const String &name,
        Dictionary<std::type_index, Ref<ComponentBase>> components);

    /**
     * Retreive the entity information based on the entity ID.
     * If the entity is not found, then return nullptr
     */
    Ref<EntityInfo> ECSGetEntity(entity_id_t id);

    /**
     * Retreive the entity id based on the name of the entity.
     * The name is case-sensitive and be the name which is generated
     *      automattically (see the core/auto_naming.hpp)
     *
     * @param name The name of the entity
     * @return The ID of the entity, if the entity is not found, then return
     *      INVALID_ENTITY_ID
     */
    entity_id_t ECSGetEntityByName(const String &name);

    /**
     * Remove all the entity from the whole system.
     */
    void ECSRemoveAllEntities();

    /**
     * Retrieve the component with the certain type from the entity.
     * If the entity is not found, then return nullptr
     */
    Ref<ComponentBase> ECSGetEntityComponent(entity_id_t id, std::type_index type);

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
     *      The enity is removed at the end of the frame.
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
} // namespace ntt

#define ECS_GET_COMPONENT(id, type) std::static_pointer_cast<type>( \
    ECSGetEntityComponent(id, typeid(type)))
#define ECS_CREATE_COMPONENT(type, ...) {typeid(type), std::make_shared<type>(__VA_ARGS__)}