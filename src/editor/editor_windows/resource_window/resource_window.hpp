#pragma once
#include "../../types.hpp"
#include "../OpenClosableWindow.hpp"
#include "../ProjectReloadWindow.hpp"

namespace ntt
{
    class ResourceWindow : public OpenClosableWindow, public ProjectReloadWindow
    {
    public:
        ResourceWindow(Ref<ProjectInfo> project, Ref<EditorConfig> config);
        ~ResourceWindow() override;

        void OnReloadProject() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

} // namespace ntt
