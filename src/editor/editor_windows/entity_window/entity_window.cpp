#include "entity_window.hpp"
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace event;

    class EntityWindow::Impl
    {
    public:
        List<entity_id_t> selectedEntities;

        void OnSelectedEntity(event_code_t code, void *sender, const EventContext &context)
        {
            selectedEntities.clear();
            selectedEntities.push_back(context.u32_data[0]);
        }

        void OnAppendEntity(event_code_t code, void *sender, const EventContext &context)
        {
            selectedEntities.push_back(context.u32_data[0]);
        }
    };

    EntityWindow::EntityWindow(Ref<ProjectInfo> project,
                               Ref<SceneInfo> scene,
                               Ref<EditorConfig> config)
        : OpenClosableWindow("Entity")
    {
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
    }

    EntityWindow::~EntityWindow() {}

    void EntityWindow::OnReloadProject()
    {
        m_impl->selectedEntities.clear();
    }

    void EntityWindow::OnReloadScene()
    {
        m_impl->selectedEntities.clear();
    }

    void EntityWindow::InitImpl()
    {
    }

    void EntityWindow::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        ImGui::Begin("Entity", p_open, flags);
        if (m_impl->selectedEntities.size() != 1)
        {
            ImGui::Text("No entity is selected");
            ImGui::End();
            return;
        }
        else
        {
            ImGui::Text("Selected entities:");
            for (auto &entity : m_impl->selectedEntities)
            {
                ImGui::Text(format("Entity: {}", entity).RawString().c_str());
            }
        }
        ImGui::End();
    }

    void EntityWindow::ShutdownImpl()
    {
    }
} // namespace ntt
