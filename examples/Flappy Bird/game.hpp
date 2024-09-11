#pragma once

#include <NTTEngine/NTTEngine.hpp>
#include "game_data.hpp"

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;

class Game
{
public:
    Game();
    ~Game();

    void Update(f32 delta);

protected:
private:
    b8 m_start = FALSE;
};