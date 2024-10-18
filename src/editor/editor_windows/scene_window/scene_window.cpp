#include "imgui.h"
#include "rlImGui.h"
#include "scene_window.hpp"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/editor/editor_utils.hpp>

namespace ntt
{
    class SceneWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<SceneInfo> scene;
        Ref<EditorConfig> config;
        EditorData editorData;

        List<entity_id_t> selectedEntities;
        List<String> chosenEntitiesNames;

        void OnEditorSelectEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            selectedEntities.clear();
            chosenEntitiesNames.clear();

            entity_id_t entityID = context.u32_data[0];
            auto entityInfo = ECSGetEntity(entityID);
            if (entityInfo == nullptr)
            {
                return;
            }
            selectedEntities.push_back(context.u32_data[0]);
            chosenEntitiesNames.push_back(entityInfo->name);
        }

        void OnEditorAppendEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            entity_id_t entityID = context.u32_data[0];
            auto entityInfo = ECSGetEntity(entityID);
            if (entityInfo == nullptr)
            {
                return;
            }
            selectedEntities.push_back(context.u32_data[0]);
            chosenEntitiesNames.push_back(entityInfo->name);
        }

        void OnEditorClearEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            selectedEntities.clear();
            chosenEntitiesNames.clear();
        }
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

        RegisterEvent(NTT_EDITOR_CHOOSE_ENTITY,
                      std::bind(&Impl::OnEditorSelectEntity, m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_APPEND_ENTITY,
                      std::bind(&Impl::OnEditorAppendEntity, m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_CLEAR_CHOSEN_ENTITY,
                      std::bind(&Impl::OnEditorClearEntity, m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));
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
        if (ImGui::Button("Delete Scene"))
        {
            m_impl->project->RemoveScene(m_impl->scene->sceneName);
            m_impl->scene->sceneName = "";
            TriggerEvent(NTT_EDITOR_SAVE_PROJECT);
            Editor_ChangeScene("");
            // m_impl->scene->SaveEntitiesInfo();
        }
        ImGui::Separator();
        if (ImGui::Button("New"))
        {
            m_impl->scene->AddEntity();
            m_impl->scene->SaveEntitiesInfo();
            m_impl->scene->ReloadEntities();
        }
        ImGui::Separator();

        b8 modified = FALSE;
        List<String> entitiesNames =
            m_impl->scene->entities
                .Map<String>([](const Ref<EntityInfo> &entity, ...)
                             { return entity->name; });

        for (auto &entityName : entitiesNames)
        {
            if (ImGui::Selectable(entityName.RawString().c_str(),
                                  m_impl->chosenEntitiesNames.Contains(entityName)))
            {
                EventContext context;
                context.u32_data[0] = ECSGetEntityByName(entityName);
                TriggerEvent(NTT_EDITOR_CHOOSE_ENTITY, nullptr, context);

                ImGui::SetWindowFocus("Entity");
            }
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
        // m_impl->scene->ReloadEntities();
    }
} // namespace ntt
