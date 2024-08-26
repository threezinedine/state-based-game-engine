#pragma once

#include <NTTEngine/IApplication.hpp>

namespace ntt
{
    class Application : public IApplication
    {
    public:
        Application(u16 screenWidth, u16 screenHeight, const char *title);
        ~Application() override;

        void Begin() override;
        bool ShouldClose() override;
        void Update(f32 deltaTime) override;
        void End() override;

    private:
        u16 m_screenWidth;
        u16 m_screenHeight;
        const char *m_title;
    };
} // namespace ntt
