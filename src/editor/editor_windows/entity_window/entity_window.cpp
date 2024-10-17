#include "entity_window.hpp"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    class EntityWindow::Impl
    {
    public:
        List<entity_id_t> selectedEntities;
        Ref<ProjectInfo> project;
        Ref<SceneInfo> scene;
        Ref<EditorConfig> config;
        EditorData editorData;
        Ref<EntityInfo> entity;

        void OnSelectedEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            selectedEntities.clear();
            selectedEntities.push_back(context.u32_data[0]);
            entity = ECSGetEntity(context.u32_data[0]);
        }

        void OnAppendEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            selectedEntities.push_back(context.u32_data[0]);
        }

        void OnClearEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            selectedEntities.clear();
            entity.reset();
        }

        void OnReloadScene(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();

            if (entity)
            {
                entity = ECSGetEntity(ECSGetEntityByName(entity->name));
            }
        }
    };

    EntityWindow::EntityWindow(Ref<ProjectInfo> project,
                               Ref<SceneInfo> scene,
                               Ref<EditorConfig> config)
        : OpenClosableWindow("Entity")
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();

        RegisterEvent(NTT_EDITOR_APPEND_ENTITY,
                      std::bind(&Impl::OnAppendEntity, m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_CHOOSE_ENTITY,
                      std::bind(&Impl::OnSelectedEntity, m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_CLEAR_CHOSEN_ENTITY,
                      std::bind(&Impl::OnClearEntity, m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_RELOAD_SCENE,
                      std::bind(&Impl::OnReloadScene, m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        m_impl->project = project;
        m_impl->scene = scene;
        m_impl->config = config;
        m_impl->editorData.project = project;
        m_impl->editorData.config = config;
        m_impl->editorData.scene = scene;
    }

    EntityWindow::~EntityWindow()
    {
        PROFILE_FUNCTION();
    }

    void EntityWindow::OnReloadProject()
    {
        PROFILE_FUNCTION();
        // m_impl->selectedEntities.clear();
    }

    void EntityWindow::OnReloadScene()
    {
        PROFILE_FUNCTION();
        // m_impl->selectedEntities.clear();
    }

    void EntityWindow::InitImpl()
    {
        PROFILE_FUNCTION();
    }

    void EntityWindow::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        PROFILE_FUNCTION();
        ImGui::Begin("Entity", p_open, flags);
        if (m_impl->selectedEntities.size() == 0)
        {
            ImGui::Text("No entity is selected");
            ImGui::End();
            return;
        }
        else if (m_impl->selectedEntities.size() > 1)
        {
            ImGui::Text("Multiple entities are selected");
            ImGui::End();
            return;
        }
        else
        {
            ImGui::Text("Selected entities:");
            if (m_impl->entity)
            {
                m_impl->entity->OnEditorUpdate(
                    [&]()
                    {
                        String entityName = m_impl->entity->name;
                        m_impl->scene->SaveEntitiesInfo();
                        m_impl->scene->RemoveAllEntities();
                        m_impl->scene->ReloadEntities();
                        m_impl->entity = ECSGetEntity(ECSGetEntityByName(entityName));
                    },
                    &m_impl->editorData);
            }
        }
        ImGui::End();
    }

    void EntityWindow::ShutdownImpl()
    {
        PROFILE_FUNCTION();
        if (m_impl->entity)
        {
            m_impl->entity.reset();
        }
    }
} // namespace ntt
