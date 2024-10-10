#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/physics/physics.hpp>

namespace ntt
{
    using namespace memory;
    using namespace physics;

    void MassComponentInit();
    void MassComponentDraw(Ref<Mass> mass);
    void MassComponentShutdown();
} // namespace ntt
