#include "MoveXController.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
    using namespace log;
    using namespace ecs;

    class MoveXController::Impl
    {
    public:
    };

    MoveXController::MoveXController(void *data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
    }

    MoveXController::~MoveXController()
    {
        PROFILE_FUNCTION();
    }

    void MoveXController::OnEnterImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveXController::OnExitImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveXController::OnUpdateImpl(f32 deltaTime)
    {
        PROFILE_FUNCTION();
    }

} // namespace ntt
