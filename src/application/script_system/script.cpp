#include <NTTEngine/application/script_system/native_system.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace log;
    using namespace memory;

    void Script::OnEnter()
    {
        OnEnterImpl();
    }

    void Script::OnExit()
    {
        OnExitImpl();
    }

    void Script::OnUpdate(f32 deltaTime)
    {
        OnUpdateImpl(deltaTime);
    }

}