#include "x_align.hpp"

#define MATCH_GAP 10

namespace ntt
{
    class XAlign::Impl
    {
    public:
        b8 isMatched;
        position_t temp;
        position_t matchedValue;

        b8 isLeftEdgeMatched = FALSE;
        b8 isRightEdgeMatched = FALSE;

        void CheckMatched(Position geo, Size size, List<position_t> points)
        {
            for (auto point : points)
            {
                if (abs(geo.x - point) < MATCH_GAP)
                {
                    matchedValue = point;
                    isMatched = TRUE;
                    isLeftEdgeMatched = FALSE;
                    isRightEdgeMatched = FALSE;
                    return;
                }

                if (abs(geo.x - size.width / 2 - point) < MATCH_GAP)
                {
                    isLeftEdgeMatched = TRUE;
                    isMatched = FALSE;
                    isRightEdgeMatched = FALSE;
                    matchedValue = point;
                    return;
                }

                if (abs(geo.x + size.width / 2 - point) < MATCH_GAP)
                {
                    isRightEdgeMatched = TRUE;
                    isMatched = FALSE;
                    isLeftEdgeMatched = FALSE;
                    matchedValue = point;
                    return;
                }
            }

            isMatched = FALSE;
            isLeftEdgeMatched = FALSE;
            isRightEdgeMatched = FALSE;
        }
    };

    XAlign::XAlign()
        : m(CreateScope<Impl>())
    {
        m->isMatched = FALSE;
        m->isLeftEdgeMatched = FALSE;
        m->isRightEdgeMatched = FALSE;
    }

    XAlign::~XAlign()
    {
    }

    b8 XAlign::IsMatched() const
    {
        return m->isMatched || m->isLeftEdgeMatched || m->isRightEdgeMatched;
    }

    position_t XAlign::GetMatchedValue() const
    {
        return m->matchedValue;
    }

    void XAlign::Update(
        Ref<Geometry> geo,
        const Position &delta,
        List<position_t> points)
    {
        if (m->isMatched || m->isLeftEdgeMatched || m->isRightEdgeMatched)
        {
            m->CheckMatched({m->temp, geo->pos.y}, geo->size, points);

            if (!m->isMatched && !m->isLeftEdgeMatched && !m->isRightEdgeMatched)
            {
                geo->pos.x = m->temp;
            }
        }
        else
        {
            m->CheckMatched(geo->pos, geo->size, points);

            if (m->isMatched || m->isLeftEdgeMatched || m->isRightEdgeMatched)
            {
                m->temp = geo->pos.x;
            }

            if (m->isMatched)
            {
                geo->pos.x = m->matchedValue;
            }

            if (m->isLeftEdgeMatched)
            {
                geo->pos.x = m->matchedValue + geo->size.width / 2;
            }

            if (m->isRightEdgeMatched)
            {
                geo->pos.x = m->matchedValue - geo->size.width / 2;
            }
        }

        if (m->isMatched || m->isLeftEdgeMatched || m->isRightEdgeMatched)
        {
            m->temp += delta.x;
        }
        else
        {
            geo->pos.x += delta.x;
        }
    }

    void XAlign::Reset()
    {
        m->isMatched = FALSE;
        m->isLeftEdgeMatched = FALSE;
        m->isRightEdgeMatched = FALSE;
    }
} // namespace ntt
