#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/defines.hpp>
#include "memplumber.h"
#include <fmt/core.h>

namespace ntt::memory
{
    class MemoryChecker
    {
    public:
        MemoryChecker();
        ~MemoryChecker();
    };
} // namespace ntt::memory

#ifdef _DEBUG
#include <NTTEngine/core/memory.hpp>
static ntt::memory::MemoryChecker memoryChecker;
#endif

namespace ntt::memory
{
    using namespace log;

    namespace
    {
        b8 s_isInitialized = FALSE;

        Dictionary<void *, PointerInfo> s_pointersInfo;
    }

    void MemoryInit()
    {
        if (s_isInitialized)
        {
            return;
        }

        s_isInitialized = TRUE;
    }

    void MemoryShutdown()
    {
        if (!s_isInitialized)
        {
            return;
        }

        s_isInitialized = FALSE;
    }

    void RegisterPointer(void *ptr, const PointerInfo &info)
    {
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The memory module is not initialized yet");
            return;
        }

        if (s_pointersInfo.Contains(ptr))
        {
            NTT_ENGINE_WARN("The pointer is already registered");
            return;
        }

        s_pointersInfo.Insert(ptr, info);
    }

    void UnregisterPointer(void *ptr)
    {
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The memory module is not initialized yet");
            return;
        }

        if (!s_pointersInfo.Contains(ptr))
        {
            NTT_ENGINE_WARN("The pointer is not registered");
            return;
        }

        s_pointersInfo.Remove(ptr);
    }

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
} // namespace ntt::memory
