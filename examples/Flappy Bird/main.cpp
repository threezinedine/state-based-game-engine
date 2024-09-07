#include <NTTEngine/main.hpp>
#include "game.hpp"

using namespace ntt;
using namespace ntt::log;
using namespace ntt::input;
using namespace ntt::event;
using namespace ntt::renderer;
using namespace ntt::audio;
using namespace ntt::ecs;
using namespace ntt::physics;

String GetSourceDir()
{
    return {"C:/Users/Acer/Games Dev/state-based-game-engine/examples/Flappy Bird/assets"};
}

Scope<Game> game;

void Begin()
{
    NTT_APP_CONFIG(LogLevel::DEBUG, LOGGER_CONSOLE);

    game = CreateScope<Game>();
}

void MainLoop(f32 delta)
{
    game->Update(delta);
}

void Close()
{
    game.reset();
}