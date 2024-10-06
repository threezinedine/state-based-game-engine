#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/assertion.hpp>

namespace ntt::memory
{
    using namespace log;

    namespace
    {
        b8 s_isInitialized = FALSE;
        u32 s_createdObjects = 0;
    }

    void MemoryInit()
    {
        PROFILE_FUNCTION();
        if (s_isInitialized)
        {
            return;
        }

        s_isInitialized = TRUE;
        s_createdObjects = 0;
    }

    void MemoryShutdown()
    {
        PROFILE_FUNCTION();

        if (!s_isInitialized)
        {
            return;
        }

        ASSERT_M(s_createdObjects == 0,
                 format("There are memory leaks in the engine: {} objects", s_createdObjects));
        s_isInitialized = FALSE;
    }

    // void RegisterPointer(void *ptr, const PointerInfo &info)
    // {
    //     PROFILE_FUNCTION();

    //     if (!s_isInitialized)
    //     {
    //         NTT_ENGINE_ERROR("The memory module is not initialized yet");
    //         return;
    //     }

    //     if (s_pointersInfo.Contains(ptr))
    //     {
    //         NTT_ENGINE_WARN("The pointer is already registered");
    //         return;
    //     }

    //     s_pointersInfo[ptr] = info;
    // }

    // void UnregisterPointer(void *ptr)
    // {
    //     PROFILE_FUNCTION();

    //     if (!s_isInitialized)
    //     {
    //         NTT_ENGINE_ERROR("The memory module is not initialized yet");
    //         return;
    //     }

    //     if (!s_pointersInfo.Contains(ptr))
    //     {
    //         NTT_ENGINE_WARN("The pointer is not registered");
    //         return;
    //     }

    //     s_pointersInfo.erase(ptr);
    // }

    void AllocateCalled()
    {
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The memory module is not initialized yet");
            return;
        }

        s_createdObjects++;
    }

    void DeallocateCalled()
    {
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The memory module is not initialized yet");
            return;
        }

        s_createdObjects--;
    }
} // namespace ntt::memory
