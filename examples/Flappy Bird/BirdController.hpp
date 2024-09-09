#pragma once

#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::memory;
using namespace ntt::log;
using namespace ntt::script;

class BirdController : public Script
{
public:
    void OnCreate();
    void OnDestroy();
    void OnUpdate(f32 deltaTime);
};