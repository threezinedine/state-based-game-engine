#pragma once

namespace ntt
{
    template <typename T>
    T GetMin(T a, T b)
    {
        return a < b ? a : b;
    }
} // namespace ntt
