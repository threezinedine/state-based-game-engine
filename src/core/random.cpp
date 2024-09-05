#include <NTTEngine/core/random.hpp>
#include <random>

namespace ntt
{
    u8 Random(u8 min, u8 max)
    {
        return min + std::rand() % (max - min + 1);
    }

    u8 Random(u8 max)
    {
        return std::rand() % (max + 1);
    }

    u16 Random(u16 min, u16 max)
    {
        return min + std::rand() % (max - min + 1);
    }

    u16 Random(u16 max)
    {
        return std::rand() % (max + 1);
    }

    u32 Random(u32 min, u32 max)
    {
        return min + std::rand() % (max - min + 1);
    }

    u32 Random(u32 max)
    {
        return std::rand() % (max + 1);
    }

    u64 Random(u64 min, u64 max)
    {
        return min + std::rand() % (max - min + 1);
    }

    u64 Random(u64 max)
    {
        return std::rand() % (max + 1);
    }

    f32 Random(f32 min, f32 max)
    {
        return min + static_cast<f32>(std::rand()) / (static_cast<f32>(RAND_MAX / (max - min)));
    }

    f32 Random(f32 max)
    {
        return static_cast<f32>(std::rand()) / (static_cast<f32>(RAND_MAX / max));
    }

    f64 Random(f64 min, f64 max)
    {
        return min + static_cast<f64>(std::rand()) / (static_cast<f64>(RAND_MAX / (max - min)));
    }

    f64 Random(f64 max)
    {
        return static_cast<f64>(std::rand()) / (static_cast<f64>(RAND_MAX / max));
    }
} // namespace ntt
