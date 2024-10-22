#include "x_align.hpp"

namespace ntt
{
    class XAlign::Impl
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

    XAlign::XAlign()
        : m(CreateScope<Impl>())
    {
        m->isMatched = FALSE;
    }

    XAlign::~XAlign()
    {
    }

    b8 XAlign::IsMatched() const
    {
        return m->isMatched;
    }

    position_t XAlign::GetMatchedValue() const
    {
        return m->matchedValue;
    }

    void XAlign::Update(
        Position &geo,
        const Position &delta,
        List<position_t> points)
    {
        if (m->isMatched)
        {
            m->CheckMatched({m->temp, geo.y}, points);

            if (!m->isMatched)
            {
                geo.x = m->temp;
            }
        }
        else
        {
            m->CheckMatched(geo, points);

            if (m->isMatched)
            {
                m->temp = geo.x;
                geo.x = m->matchedValue;
            }
        }

        if (m->isMatched)
        {
            m->temp += delta.x;
        }
        else
        {
            geo.x += delta.x;
        }
    }

    void XAlign::Reset()
    {
        m->isMatched = FALSE;
    }
} // namespace ntt
