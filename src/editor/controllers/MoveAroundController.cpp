#include "MoveAroundController.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

namespace ntt
{
    using namespace log;
    using namespace ecs;
    using namespace input;

    class MoveAroundController::Impl
    {
    public:
        entity_id_t entity;
    };

    MoveAroundController::MoveAroundController(void *data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->entity = *static_cast<entity_id_t *>(data);
    }

    MoveAroundController::~MoveAroundController()
    {
        PROFILE_FUNCTION();
    }

    void MoveAroundController::OnEnterImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveAroundController::OnExitImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveAroundController::OnUpdateImpl(f32 deltaTime)
    {
        PROFILE_FUNCTION();
    }

    void MoveAroundController::OnHover(HoveringContext &)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_DOWN))
        {
            // NTT_ENGINE_DEBUG("MoveAroundController: Left button pressed");
            auto geo = GetComponent<Geometry>();
            auto entGeo = ECS_GET_COMPONENT(m_impl->entity, Geometry);

            if (entGeo == nullptr)
            {
                NTT_ENGINE_WARN("The entity with ID {} does not have a geometry component",
                                m_impl->entity);
                return;
            }

            if (geo == nullptr)
            {
                NTT_ENGINE_WARN("The entity with ID {} does not have a geometry component",
                                GetEntity());
                return;
            }

            auto mouse = GetMousePosition();

            geo->x = mouse.x;
            geo->y = mouse.y;
            entGeo->x = mouse.x;
            entGeo->y = mouse.y;
        }
    }

} // namespace ntt
