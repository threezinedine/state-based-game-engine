#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/structures/size.hpp>

namespace ntt
{
    using camera_id_t = u32;
    constexpr camera_id_t INVALID_CAMERA_ID = u32{0} - 1;

    struct Camera
    {
        /**
         * ========================
         * ||                    ||
         * ||                    ||
         * ||        Pos         ||
         * ||                    ||
         * ||                    ||
         * ========================
         *
         * No matter what is the position of the camera, that poit will be displayed at the
         *      center of the outputFrame.
         */
        Position camPos; ///< The position of the camera (the center of the outputFrame below)

        f32 camZoom = 1.0f; ///< Larger value will zoom in, smaller value will zoom out

        Position ouputPos; ///< Each type of camera has its own position.
        Size outputSize;   ///< The size of the frame where the output of the camera will be displayed

        Camera(position_t outputPosX, position_t outputPosY, size_t outputWidth, size_t outputHeight);

        /**
         * The camera will be reset to the default where the the camera will capture the outputFrame which
         *      will match 1:1 with the worldFrame.
         */
        void Reset();

        position_t TransformX(position_t x) const;
        position_t TransformY(position_t y) const;

        ntt_size_t TransformWidth(ntt_size_t width) const;
        ntt_size_t TransformHeight(ntt_size_t height) const;

        position_t ReverseTransformX(position_t x) const;
        position_t ReverseTransformY(position_t y) const;

        ntt_size_t ReverseTransformWidth(ntt_size_t width) const;
        ntt_size_t ReverseTransformHeight(ntt_size_t height) const;

        /**
         * Shift the camera so that the startPos (original position) will be placed
         *      at the targetPos (output frame), the zoom will be the same but the
         *      camera's position will be changed.
         */
        void ShiftCamera(Position startPos, Position targetPos);
    };
} // namespace ntt
