#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/defines.hpp>
#include <fmt/core.h>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/assertion.hpp>

#if defined(_DEBUG) && not defined(NTTENGINE_SHARED)
#include "memplumber.h"

namespace ntt::memory
{
    class MemoryChecker
    {
    public:
        MemoryChecker();
        ~MemoryChecker();
    };
} // namespace ntt::memory

static ntt::memory::MemoryChecker memoryChecker;
#endif

namespace ntt::memory
{
    using namespace log;

    namespace
    {
        b8 s_isInitialized = FALSE;

        Dictionary<void *, PointerInfo> s_pointersInfo;
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

#if defined(_DEBUG) && not defined(NTTENGINE_SHARED)
    MemoryChecker::MemoryChecker()
    {
        MemPlumber::start();
    }

    MemoryChecker::~MemoryChecker()
    {
        ::std::size_t memLeakCount;
        u64 memLeakSize;

        MemPlumber::memLeakCheck(memLeakCount, memLeakSize, true);

        if (memLeakCount > 0)
        {
            printf("Memory leaks detected: %zu leaks with a total of %zu bytes\n",
                   memLeakCount, memLeakSize);
        }

        auto forEach = [](const void *ptr, PointerInfo &info)
        {
            fmt::print("Memory leak detected at {}:{} with size {}\n",
                       info.file,
                       info.line,
                       info.size);
        };

        s_pointersInfo.ForEach(forEach);
    }
#endif
} // namespace ntt::memory
