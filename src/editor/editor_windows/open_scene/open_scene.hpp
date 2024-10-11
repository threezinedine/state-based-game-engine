#include "../OpenClosableWindow.hpp"
#include "../../types.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    class OpenSceneWindow : public OpenClosableWindow
    {
    public:
        OpenSceneWindow(Ref<ProjectInfo> project,
                        Ref<EditorConfig> config,
                        Ref<SceneInfo> scene);
        ~OpenSceneWindow();

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
