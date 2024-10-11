#include "../OpenClosableWindow.hpp"
#include "../SceneReloadWindow.hpp"
#include "../ProjectReloadWindow.hpp"
#include "../../types.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

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
        void UpdateImpl(b8 *p_open) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
