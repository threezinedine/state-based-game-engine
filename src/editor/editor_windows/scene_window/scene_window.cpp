#include "imgui.h"
#include "rlImGui.h"
#include "scene_window.hpp"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/renderer/renderer.hpp>

namespace ntt
{
    using namespace event;
    using namespace renderer;
    using namespace ecs;

    class SceneWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<SceneInfo> scene;
        Ref<EditorConfig> config;
        EditorData editorData;
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

        m_impl->editorData.project = project;
        m_impl->editorData.config = config;
        m_impl->editorData.scene = scene;
    }

    SceneWindow::~SceneWindow() {}

    void SceneWindow::InitImpl()
    {
    }

    void SceneWindow::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        ImGui::Begin("Scene", p_open, flags);
        if (m_impl->scene->sceneName == "")
        {
            ImGui::Text("Non scene is selected");
            ImGui::End();
            return;
        }

        ImGui::Text(format("Scene: {}", m_impl->scene->sceneName).RawString().c_str());
        ImGui::Separator();
        if (ImGui::Button("New"))
        {
            m_impl->scene->AddEntity();
            m_impl->scene->SaveEntitiesInfo();
        }
        ImGui::Separator();

        ImGui::SetNextItemOpen(TRUE, ImGuiCond_Once);
        if (ImGui::TreeNode("Entities"))
        {
            ImGui::SetNextItemOpen(TRUE, ImGuiCond_Once);
            for (auto &entity : m_impl->scene->entities)
            {
                ImGui::PushID(format("Entity: {}",
                                     entity->name.RawString())
                                  .RawString()
                                  .c_str());

                entity->OnEditorUpdate([&]()
                                       { m_impl->scene->SaveEntitiesInfo(); },
                                       &m_impl->editorData);

                ImGui::PopID();
            }

            ImGui::TreePop();
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
        m_impl->scene->ReloadEntities();
    }
} // namespace ntt
