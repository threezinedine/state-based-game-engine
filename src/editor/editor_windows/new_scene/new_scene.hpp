#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/editor/OpenClosableWindow.hpp>

namespace ntt
{
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
