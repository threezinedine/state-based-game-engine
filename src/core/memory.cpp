#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/defines.hpp>
#include "memplumber.h"
#include <cstdio>

#ifdef _DEBUG
#include <NTTEngine/core/memory.hpp>
static ntt::memory::MemoryChecker memoryChecker;
#endif

namespace ntt::memory
{
    using namespace log;

    void MemoryInit()
    {
    }

    void MemoryShutdown()
    {
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
    }
} // namespace ntt::memory
