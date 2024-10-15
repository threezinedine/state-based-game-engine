#pragma once
#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/editor/ProjectReloadWindow.hpp>
#include <NTTEngine/editor/SceneReloadWindow.hpp>

namespace ntt
{
    using namespace ecs;

    class EntityWindow : public OpenClosableWindow,
                         public ProjectReloadWindow,
                         public SceneReloadWindow
    {
    public:
        EntityWindow(Ref<ProjectInfo> project,
                     Ref<SceneInfo> scene,
                     Ref<EditorConfig> config);
        ~EntityWindow();

        void OnReloadProject() override;
        void OnReloadScene() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
