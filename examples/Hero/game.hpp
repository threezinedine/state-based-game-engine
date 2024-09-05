#pragma once

#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;

enum GameState
{
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING,
    GAME_STATE_OVER,
};

class Game
{
public:
    Game();
    ~Game();

    void Update(f32 delta);
    void RandomizePipe(position_t posX);

protected:
    void PipeHandling(f32 delta, entity_id_t id, List<entity_id_t> others);
    void OnBirdCollide(List<entity_id_t> others);

private:
    u32 m_createdPipeCount;

    entity_id_t m_bird;
    List<entity_id_t> m_pipes;
    GameState m_state;

    Timer m_pipeTimer;
    b8 m_start = FALSE;
};