#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/structures/size.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "GraphicInterface.hpp"
#include <NTTEngine/core/parser/json.hpp>

namespace ntt
{
    /**
     * Component which store the rendering information
     *      of position, size of the entity, etc.
     */
    struct Geometry : public ComponentBase
    {
        Position pos;
        Size size;
        f32 rotation;

        u8 priority;
        b8 keepRatio;
        RGBAColor color;

        // should not be used by user, only used by the renderer and editor
        Size originalSize;

        Geometry(position_t x = POSITION_DEFAULT, position_t y = POSITION_DEFAULT,
                 ntt_size_t width = SIZE_DEFAULT, ntt_size_t height = SIZE_DEFAULT,
                 f32 rotation = 0.0f, u8 priority = PRIORITY_0,
                 RGBAColor color = NTT_WHITE,
                 b8 keepRatio = FALSE)
            : pos(x, y), size(width, height), rotation(rotation), priority(priority),
              keepRatio(keepRatio), color(color),
              originalSize(100, 100)
        {
        }

        virtual void TurnOff() override;
        virtual void TurnOn() override;

        String GetName() const override;
        JSON ToJSON() const override;
        void FromJSON(const JSON &json) override;
        void OnEditorUpdate(std::function<void()> onChanged = nullptr, void *data = nullptr) override;
    };
} // namespace ntt
