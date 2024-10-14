#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/editor/types.hpp>
#include "../OpenClosableWindow.hpp"
#include "../ProjectReloadWindow.hpp"
#include "../SceneReloadWindow.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    class SceneWindow : public OpenClosableWindow,
                        public ProjectReloadWindow,
                        public SceneReloadWindow
    {
    public:
        SceneWindow(Ref<ProjectInfo> project, Ref<SceneInfo> scene, Ref<EditorConfig> config);
        ~SceneWindow() override;

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
