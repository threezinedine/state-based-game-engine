#pragma once
#include <NTTEngine/defines.hpp>
#include <functional>
#include <NTTEngine/core/parser/json.hpp>

namespace ntt
{
    /**
     * If need to change the type of the position, then
     *      does not need to change any code in the engine
     */
    using position_t = f32;
    constexpr position_t POSITION_DEFAULT = 0.0f;

    /**
     * This is used for representing a position
     *      (in this engine its unit is pixel)
     */
    struct Position
    {
        position_t x; ///< The x position in pixel
        position_t y; ///< The y position in pixel

        Position(position_t x = POSITION_DEFAULT, position_t y = POSITION_DEFAULT) : x(x), y(y) {}

        void FromJSON(const JSON &json);
        JSON ToJSON() const;
        void OnEditorUpdate(std::function<void()> onChanged = nullptr);
    };

    using PositionTransform = std::function<Position(const Position &pos)>;
    // typedef Position (*PositionTransform)(const Position &pos);
} // namespace ntt
