#pragma once
#include "../../types.hpp"
#include "../OpenClosableWindow.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    class SettingWindow : public OpenClosableWindow
    {
    public:
        SettingWindow(Ref<ProjectInfo> project, Ref<EditorConfig> config);
        ~SettingWindow() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
