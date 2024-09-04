#pragma once
#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;

struct Geometry : public ComponentBase
{
    position_t x;
    position_t y;
    ntt::size_t width;
    ntt::size_t height;

    Geometry(position_t x = 0, position_t y = 0,
             ntt::size_t width = SIZE_DEFAULT,
             ntt::size_t height = SIZE_DEFAULT)
        : x(x), y(y), width(width), height(height)
    {
    }
};

struct Texture : public ComponentBase
{
    texture_id_t id;
    u8 rowIndex;
    u8 colIndex;

    Texture(texture_id_t id, u8 rowIndex = 0, u8 colIndex = 0)
        : id(id), rowIndex(rowIndex), colIndex(colIndex)
    {
    }
};

struct Velocity : public ComponentBase
{
    f32 x;
    f32 y;

    Velocity(f32 x, f32 y)
        : x(x), y(y)
    {
    }
};

struct Bouncing : public ComponentBase
{
    b8 isBounce;

    Bouncing(b8 isBounce = FALSE)
        : isBounce(isBounce) {}
};

struct Hovering : public ComponentBase
{
    b8 isHoverred;
    std::function<void(f32, entity_id_t)> onEnter;
    std::function<void(f32, entity_id_t)> onBlur;

    Hovering(b8 isHoverred = FALSE)
        : isHoverred(isHoverred) {}
};