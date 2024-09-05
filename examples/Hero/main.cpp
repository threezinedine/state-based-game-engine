#include <NTTEngine/main.hpp>
#include "bird.hpp"

using namespace ntt;
using namespace ntt::log;
using namespace ntt::input;
using namespace ntt::event;
using namespace ntt::renderer;
using namespace ntt::audio;
using namespace ntt::ecs;

AdditionalData CreateAdditionalData()
{
    return {"C:/Users/Acer/Games Dev/state-based-game-engine/examples/Hero/assets"};
}

void Begin()
{
    Bird bird;

    bird.CreateBird();
}

void MainLoop(f32 delta)
{
}

void Close()
{
}