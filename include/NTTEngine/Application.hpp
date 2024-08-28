#pragma once
#include "defines.hpp"
#include "core/memory.hpp"

namespace ntt
{
    using namespace memory;

    class Application
    {
    public:
        virtual ~Application() = default;

        virtual void Begin() = 0;
        virtual bool ShouldClose() = 0;
        virtual void Update(f32 deltaTime) = 0;
        virtual void End() = 0;
    };

    Scope<Application> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title);
} // namespace ntt
