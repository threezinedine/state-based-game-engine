#include <gtest/gtest.h>
#include <NTTEngine/core/profiling.hpp>

// #include <NTTEngine/core/memory.hpp>
// ntt::memory::MemoryChecker checker;

int main(int argc, char **argv)
{
    ntt::ProfilingInit("profiling", TRUE);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}