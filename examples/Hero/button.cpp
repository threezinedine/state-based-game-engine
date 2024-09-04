#include "button.hpp"
#include "component_defs.hpp"

using namespace ntt::ecs;
using namespace ntt::input;

void ButtonSystem::Update(f32 delta, entity_id_t id)
{
    auto hovering = ECS_GET_COMPONENT(id, Hovering);
    auto geo = ECS_GET_COMPONENT(id, Geometry);
    auto btnState = ECS_GET_COMPONENT(id, ButtonComponent);
    auto texture = ECS_GET_COMPONENT(id, Texture);

    if (hovering->isHoverred)
    {
        if (CheckState(MouseButton::NTT_BUTTON_LEFT, InputState::NTT_DOWN))
        {
            texture->rowIndex = 2;
        }
        else
        {
            texture->rowIndex = 1;
        }
    }
    else
    {
        texture->rowIndex = 0;
    }
}