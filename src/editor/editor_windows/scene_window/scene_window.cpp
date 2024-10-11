#include "imgui.h"
#include "rlImGui.h"
#include "scene_window.hpp"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
    using namespace event;

    class SceneWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<SceneInfo> scene;
        Ref<EditorConfig> config;
    };

    SceneWindow::SceneWindow(Ref<ProjectInfo> project,
                             Ref<SceneInfo> scene,
                             Ref<EditorConfig> config)
        : OpenClosableWindow("Scene")
    {
        m_impl = CreateScope<Impl>();
        m_impl->project = project;
        m_impl->scene = scene;
        m_impl->config = config;
    }

    SceneWindow::~SceneWindow() {}

    void SceneWindow::InitImpl()
    {
    }

    void SceneWindow::UpdateImpl(b8 *p_open)
    {
        ImGui::Begin("Scene", p_open);
        if (m_impl->scene->sceneName == "")
        {
            ImGui::Text("Non scene is selected");
        }
        else
        {
            ImGui::Text(format("Scene: {}", m_impl->scene->sceneName).RawString().c_str());
            ImGui::Separator();
        }
        ImGui::End();
    }

    void SceneWindow::ShutdownImpl()
    {
    }

    void SceneWindow::OnReloadProject()
    {
    }

    void SceneWindow::OnReloadScene()
    {
    }
} // namespace ntt
