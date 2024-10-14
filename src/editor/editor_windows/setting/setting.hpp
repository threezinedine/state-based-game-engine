#pragma once
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/editor/OpenClosableWindow.hpp>
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
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
