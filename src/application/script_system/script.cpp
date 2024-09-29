#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace log;
    using namespace memory;

    void Script::OnEnter()
    {
        PROFILE_FUNCTION();
        OnEnterImpl();
    }

    void Script::OnExit()
    {
        PROFILE_FUNCTION();
        Delete();
    }

    void Script::OnUpdate(f32 deltaTime)
    {
        PROFILE_FUNCTION();
        OnUpdateImpl(deltaTime);
    }

}