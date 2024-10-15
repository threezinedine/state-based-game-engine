#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include <NTTEngine/editor/ProjectReloadWindow.hpp>
#include <NTTEngine/editor/SceneReloadWindow.hpp>
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    class OpenSceneWindow : public OpenClosableWindow,
                            public ProjectReloadWindow,
                            public SceneReloadWindow
    {
    public:
        OpenSceneWindow(Ref<ProjectInfo> project,
                        Ref<EditorConfig> config,
                        Ref<SceneInfo> scene);
        ~OpenSceneWindow();

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
