#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/structures/list.hpp>

namespace ntt
{
    class AlignItem
    {
    public:
        virtual void Update(
            Position &geo,
            const Position &delta,
            List<position_t> points) = 0;

        virtual b8 IsMatched() const = 0;
        virtual position_t GetMatchedValue() const = 0;
        virtual void Reset() = 0;
    };
} // namespace ntt
