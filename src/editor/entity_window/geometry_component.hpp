#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace renderer;
    using namespace memory;

    void GeometryComponentInit();
    void GeometryComponentDraw(Ref<Geometry> geo);
    void GeometryComponentShutdown();
} // namespace ntt
