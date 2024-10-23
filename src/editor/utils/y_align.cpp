#include "y_align.hpp"

#define MATCH_GAP 10

namespace ntt
{
    class YAlign::Impl
    {
    public:
        b8 isMatched;
        position_t temp;
        position_t matchedValue;

        b8 isTopEdgeMatched = FALSE;
        b8 isBottomEdgeMatched = FALSE;

        void CheckMatched(Position geo, Size size, List<position_t> points)
        {
            for (auto point : points)
            {
                if (abs(geo.y - point) < MATCH_GAP)
                {
                    matchedValue = point;
                    isMatched = TRUE;
                    isTopEdgeMatched = FALSE;
                    isBottomEdgeMatched = FALSE;
                    return;
                }

                if (abs(geo.y - size.height / 2 - point) < MATCH_GAP)
                {
                    isTopEdgeMatched = TRUE;
                    isMatched = FALSE;
                    isBottomEdgeMatched = FALSE;
                    matchedValue = point;
                    return;
                }

                if (abs(geo.y + size.height / 2 - point) < MATCH_GAP)
                {
                    isBottomEdgeMatched = TRUE;
                    isMatched = FALSE;
                    isTopEdgeMatched = FALSE;
                    matchedValue = point;
                    return;
                }
            }

            isMatched = FALSE;
            isTopEdgeMatched = FALSE;
            isBottomEdgeMatched = FALSE;
        }
    };

    YAlign::YAlign()
        : m(CreateScope<Impl>())
    {
        m->isMatched = FALSE;
        m->isBottomEdgeMatched = FALSE;
        m->isTopEdgeMatched = FALSE;
    }

    YAlign::~YAlign()
    {
    }

    b8 YAlign::IsMatched() const
    {
        return m->isMatched || m->isTopEdgeMatched || m->isBottomEdgeMatched;
    }

    position_t YAlign::GetMatchedValue() const
    {
        return m->matchedValue;
    }

    void YAlign::Update(
        Ref<Geometry> geo,
        const Position &delta,
        List<position_t> points)
    {
        if (m->isMatched || m->isBottomEdgeMatched || m->isTopEdgeMatched)
        {
            m->CheckMatched({geo->pos.x, m->temp}, geo->size, points);

            if (!m->isMatched && !m->isTopEdgeMatched && !m->isBottomEdgeMatched)
            {
                geo->pos.y = m->temp;
            }
        }
        else
        {
            m->CheckMatched(geo->pos, geo->size, points);

            if (m->isMatched || m->isTopEdgeMatched || m->isBottomEdgeMatched)
            {
                m->temp = geo->pos.y;
            }

            if (m->isMatched)
            {
                geo->pos.y = m->matchedValue;
            }

            if (m->isTopEdgeMatched)
            {
                geo->pos.y = m->matchedValue + geo->size.height / 2;
            }

            if (m->isBottomEdgeMatched)
            {
                geo->pos.y = m->matchedValue - geo->size.height / 2;
            }
        }

        if (m->isMatched || m->isTopEdgeMatched || m->isBottomEdgeMatched)
        {
            m->temp += delta.y;
        }
        else
        {
            geo->pos.y += delta.y;
        }
    }

    void YAlign::Reset()
    {
        m->isMatched = FALSE;
        m->isBottomEdgeMatched = FALSE;
        m->isTopEdgeMatched = FALSE;
    }
} // namespace ntt
