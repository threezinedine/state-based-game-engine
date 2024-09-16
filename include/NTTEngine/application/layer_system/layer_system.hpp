#pragma once
#include <NTTEngine/defines.hpp>
#include "layer_types.hpp"

namespace ntt
{
    /**
     * Staring of the layer system, should be called in the beginning
     *      of the application, game layer will be the default layer
     *      and be turned on automatically.
     */
    void LayerInit();

    /**
     * The beginning of a new layer inside the system, the new layer will
     *      be on top of the previous layers, all entities which are created
     *      after this new layer will be attached to this layer.
     * User can begin a layer multiple times, but if the layer is created, then
     *      the old layer will added new entities to the existed layer.
     *
     * @example:
     * ```C++
     *      BeginLayer(LayerType::GAME_LAYER);
     *
     *      ECS_CREATE_ENTITY("Player", {
     *          {typeid(TransformComponent), CREATE_REF(TransformComponent, 0.0f, 0.0f, 0.0f)},
     *          {typeid(SpriteComponent), CREATE_REF(SpriteComponent, "player.png")}
     *      }); // now the "Player" entity is attached to the GAME_LAYER and the
     *          // game layer will be the lowest layer in the system
     *
     *      BeginLayer(LayerType::UI_LAYER);
     *      ECS_CREATE_ENTITY("Button", {
     *          {typeid(TransformComponent), CREATE_REF(TransformComponent, 0.0f, 0.0f, 0.0f)},
     *          {typeid(SpriteComponent), CREATE_REF(SpriteComponent, "button.png")}
     *      }); // now the "Button" entity is attached to the UI_LAYER and the
     *          // UI layer will be on top of the GAME_LAYER
     *
     *      BeginLayer(LayerType::GAME_LAYER);
     *      ECS_CREATE_ENTITY("Enemy", {
     *          {typeid(TransformComponent), CREATE_REF(TransformComponent, 0.0f, 0.0f, 0.0f)},
     *          {typeid(SpriteComponent), CREATE_REF(SpriteComponent, "enemy.png")}
     *      }); // now the "Enemy" entity is attached to the GAME_LAYER and the
     *          // game layer is still the lowest layer in the system
     * ```
     */
    void BeginLayer(LayerType type);

    /**
     * Called every frame in the game loop, update all the layers
     */
    void LayerUpdate(f32 deltaTime);

    /**
     * Staring drawing the layers on the screen
     *      if the layer is already visible then
     *      the layer will be drawn on the screen
     * Other layers will be hidden
     */
    void LayerMakeVisible(LayerType type);

    /**
     * Release all needed resources of the layer system
     *      when the application is closed
     */
    void LayerShutdown();
} // namespace ntt
