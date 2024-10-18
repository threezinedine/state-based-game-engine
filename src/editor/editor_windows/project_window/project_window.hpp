#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include <NTTEngine/editor/ProjectReloadWindow.hpp>
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    class ProjectWindow : public OpenClosableWindow,
                          public ProjectReloadWindow
    {
    public:
        ProjectWindow(Ref<ProjectInfo> project);
        ~ProjectWindow() override;

        void OnReloadProject() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
