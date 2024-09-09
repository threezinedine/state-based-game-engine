#pragma once

#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::script;

void CreatePipe(
    position_t posX,
    u16 &score);

void ResetPipe();
void StopPipe();