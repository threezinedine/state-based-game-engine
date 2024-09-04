#pragma once
#include <NTTEngine/NTTEngine.hpp>
#include "component_defs.hpp"

using namespace ntt::ecs;
using namespace ntt;

enum ButtonState
{
    IDLE,
    HOVER,
};

struct ButtonComponent : public ComponentBase
{
};

class ButtonSystem : public System
{
public:
    void Update(f32 delta, entity_id_t id) override;
};