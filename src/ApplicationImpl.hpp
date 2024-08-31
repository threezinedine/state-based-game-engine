#pragma once

#include <NTTEngine/Application.hpp>
#include <NTTEngine/core/time.hpp>

namespace ntt
{
    class ApplicationImpl : public Application
    {
    public:
        ApplicationImpl(u16 screenWidth, u16 screenHeight,
                        const char *title, const Phrases &phrases);
        ~ApplicationImpl() override;

        void Begin() override;
        bool ShouldClose() override;
        void Update() override;
        void End() override;

    private:
        u16 m_screenWidth;
        u16 m_screenHeight;
        const char *m_title;
        Timer m_timer;
        Phrases m_phrases;
    };
} // namespace ntt
