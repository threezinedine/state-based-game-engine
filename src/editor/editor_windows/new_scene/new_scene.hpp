#include "../OpenClosableWindow.hpp"
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    class NewSceneWindow : public OpenClosableWindow
    {
    public:
        NewSceneWindow(Ref<ProjectInfo> project,
                       Ref<EditorConfig> config,
                       Ref<SceneInfo> scene);
        ~NewSceneWindow();

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
