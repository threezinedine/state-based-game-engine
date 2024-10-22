#pragma once
#include <NTTEngine/core/memory.hpp>
#include "align_item.hpp"

namespace ntt
{
    class TopEdgeAlign : public AlignItem
    {
    public:
        TopEdgeAlign();
        ~TopEdgeAlign();

        void Update(
            Ref<Geometry> geo,
            const Position &delta,
            List<position_t> points) override;

        void Reset() override;
        b8 IsMatched() const override;
        position_t GetMatchedValue() const override;

    private:
        class Impl;
        Scope<Impl> m;
    };
} // namespace ntt
