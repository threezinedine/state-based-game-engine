#pragma once
#include "defines.hpp"
#include "core/memory.hpp"
#include <functional>

namespace ntt
{
    using namespace memory;

    struct Phrases
    {
        std::function<void()> Begin;
        std::function<void(f32)> MainLoop;
        std::function<void()> Close;
    };

    class Application
    {
    public:
        virtual ~Application() = default;

        virtual void Begin() = 0;
        virtual bool ShouldClose() = 0;
        virtual void Update() = 0;
        virtual void End() = 0;
    };

    Scope<Application> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title,
                                         const Phrases &phrases);
} // namespace ntt
