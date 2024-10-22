#include "align_item.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    class YAlign : public AlignItem
    {
    public:
        YAlign();
        ~YAlign();

        void Update(
            Position &geo,
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
