#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/platforms/stream.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    namespace
    {
        u32 s_createdObjects = 0;
    }

    void MemoryInit()
    {
        PROFILE_FUNCTION();
        s_createdObjects = 0;
    }

    void MemoryShutdown()
    {
        PROFILE_FUNCTION();
        ASSERT_STA(s_createdObjects == 0,
                   printf(
                       format(
                           "There are memory leaks in the engine: {} objects\n", s_createdObjects)
                           .RawString()
                           .c_str()));
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
        s_createdObjects++;
    }

    void DeallocateCalled()
    {
        s_createdObjects--;
    }
} // namespace ntt
