#pragma once

#include <NTTEngine/Application.hpp>

namespace ntt
{
    class ApplicationImpl : public Application
    {
    public:
        ApplicationImpl(u16 screenWidth, u16 screenHeight, const char *title);
        ~ApplicationImpl() override;

        void Begin() override;
        bool ShouldClose() override;
        void Update() override;
        void End() override;

    private:
        u16 m_screenWidth;
        u16 m_screenHeight;
        const char *m_title;
    };
} // namespace ntt
