#include "top_edge_align.hpp"

namespace ntt
{
    class TopEdgeAlign::Impl
    {
    public:
        b8 isMatched;
        position_t temp;
        position_t matchedValue;

        void CheckMatched(Position geo, List<position_t> points)
        {
            for (auto point : points)
            {
                if (abs(geo.x - point) < 20)
                {
                    matchedValue = point;
                    isMatched = TRUE;
                    return;
                }
            }

            isMatched = FALSE;
        }
    };

    TopEdgeAlign::TopEdgeAlign()
        : m(CreateScope<Impl>())
    {
    }

    TopEdgeAlign::~TopEdgeAlign()
    {
    }

    void TopEdgeAlign::Update(
        Ref<Geometry> geo,
        const Position &delta,
        List<position_t> points)
    {
    }

    void TopEdgeAlign::Reset()
    {
    }

    b8 TopEdgeAlign::IsMatched() const
    {
        m->isMatched = FALSE;
    }

    position_t TopEdgeAlign::GetMatchedValue() const
    {
        return m->matchedValue;
    }
} // namespace ntt
