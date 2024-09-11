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
    void RandomizePipe(position_t posX);

protected:
    void HandleScore(f32 delta, entity_id_t id);
    void PipeHandling(f32 delta, entity_id_t id);
    void OnBirdCollide(List<entity_id_t> others);

private:
    b8 m_start = FALSE;
};