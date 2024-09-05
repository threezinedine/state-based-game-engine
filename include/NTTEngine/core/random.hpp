#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    u8 Random(u8 min, u8 max);
    u8 Random(u8 max);

    u16 Random(u16 min, u16 max);
    u16 Random(u16 max);

    u32 Random(u32 min, u32 max);
    u32 Random(u32 max);

    u64 Random(u64 min, u64 max);
    u64 Random(u64 max);

    f32 Random(f32 min, f32 max);
    f32 Random(f32 max);

    f64 Random(f64 min, f64 max);
    f64 Random(f64 max);
} // namespace ntt
