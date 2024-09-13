#pragma once

#include <NTTEngine/defines.hpp>

namespace ntt
{
    /**
     * Based class for all the layers in the engine
     *      but it's hidden from the user, only the
     *      engine developer can use this class
     */
    class Layer
    {
    public:
        virtual ~Layer() = default;

        /**
         * Turn on all the logic system of the layer like Physics,
         *      Collision (Rendering is not included)
         */
        virtual void Activate() = 0;

        /**
         * Turn off all the logic system of the layer like Physics,
         *      Collision (Rendering is not included)
         */
        virtual void Deactivate() = 0;

        /**
         * Called every frame in the game loop
         */
        virtual void Update(f32 deltaTime) = 0;

        /**
         * Update the layer again if the layer is invisible
         */
        virtual void MakeVisible() = 0;

        /**
         * Remove the layer (not updated and not drawn on the screen)
         */
        virtual void MakeInvisible() = 0;
    };
} // namespace ntt
