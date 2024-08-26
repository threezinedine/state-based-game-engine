#pragma once
#include "defines.hpp"
#include "pointers.hpp"

namespace ntt
{
    class IApplication
    {
    public:
        virtual ~IApplication() = default;

        virtual void Begin() = 0;
        virtual bool ShouldClose() = 0;
        virtual void Update(f32 deltaTime) = 0;
        virtual void End() = 0;
    };

    Scope<IApplication> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title);
} // namespace ntt
