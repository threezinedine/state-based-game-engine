#include "y_align.hpp"

namespace ntt
{
    class YAlign::Impl
    {
    public:
        b8 isMatched;
        position_t temp;
        position_t matchedValue;

        void CheckMatched(Position geo, List<position_t> points)
        {
            for (auto point : points)
            {
                if (abs(geo.y - point) < 20)
                {
                    matchedValue = point;
                    isMatched = TRUE;
                    return;
                }
            }

            isMatched = FALSE;
        }
    };

    YAlign::YAlign()
        : m(CreateScope<Impl>())
    {
        m->isMatched = FALSE;
    }

    YAlign::~YAlign()
    {
    }

    b8 YAlign::IsMatched() const
    {
        return m->isMatched;
    }

    position_t YAlign::GetMatchedValue() const
    {
        return m->matchedValue;
    }

    void YAlign::Update(
        Position &geo,
        const Position &delta,
        List<position_t> points)
    {
        if (m->isMatched)
        {
            m->CheckMatched({geo.x, m->temp}, points);

            if (!m->isMatched)
            {
                geo.y = m->temp;
            }
        }
        else
        {
            m->CheckMatched(geo, points);

            if (m->isMatched)
            {
                m->temp = geo.y;
                geo.y = m->matchedValue;
            }
        }

        if (m->isMatched)
        {
            m->temp += delta.y;
        }
        else
        {
            geo.y += delta.y;
        }
    }

    void YAlign::Reset()
    {
        m->isMatched = FALSE;
    }
} // namespace ntt
