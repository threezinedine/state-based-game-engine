#include "defs.hpp"
#include <NTTEngine/NTTEngine.hpp>
#define SCORE_DIGIT_GAP 1

class ScoreBoard : public Script
{
public:
    ScoreBoard(void *data)
    {
    }

    ~ScoreBoard()
    {
    }

protected:
    void OnEnterImpl()
    {
        auto windowSize = GetWindowSize();

        m_hunderedNumber = ECSCreateEntity(
            "hundered",
            {
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 - 50, 50, 20, 20),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
            });

        m_tenNumber = ECSCreateEntity(
            "ten",
            {
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, 50, 20, 20),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
            });

        m_oneNumber = ECSCreateEntity(
            "one",
            {
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 + 50, 50, 20, 20),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
            });

        Subscribe(SCORE_CHANGED_EVENT);

        TriggerEvent(SCORE_CHANGED_EVENT, nullptr, {0});
    }

    void OnExitImpl()
    {
        ECSDeleteEntity(m_hunderedNumber);
        ECSDeleteEntity(m_tenNumber);
        ECSDeleteEntity(m_oneNumber);
    }

    void OnEvent(event_code_t code, void *sender, const EventContext &context) override
    {
        switch (code)
        {
        case SCORE_CHANGED_EVENT:
            OnScoreChanged(context.u16_data[0]);
            break;
        }
    }

    void OnScoreChanged(u16 score)
    {
        auto geo = GetComponent<Geometry>();
        auto hundredGeo = ECS_GET_COMPONENT(m_hunderedNumber, Geometry);
        auto tenGeo = ECS_GET_COMPONENT(m_tenNumber, Geometry);
        auto oneGeo = ECS_GET_COMPONENT(m_oneNumber, Geometry);

        auto hunderedText = ECS_GET_COMPONENT(m_hunderedNumber, Texture);
        auto tenText = ECS_GET_COMPONENT(m_tenNumber, Texture);
        auto oneText = ECS_GET_COMPONENT(m_oneNumber, Texture);

        hundredGeo->y = geo->y;
        tenGeo->y = geo->y;
        oneGeo->y = geo->y;

        if (score < 100)
        {
            ECSSetComponentActive(m_hunderedNumber, typeid(Texture), FALSE);
        }
        else
        {
            ECSSetComponentActive(m_hunderedNumber, typeid(Texture), TRUE);

            hundredGeo->x = geo->x;
            tenGeo->x = geo->x + hundredGeo->width + SCORE_DIGIT_GAP;
            oneGeo->x = geo->x + hundredGeo->width + tenGeo->width + SCORE_DIGIT_GAP * 2;

            hunderedText->colIndex = static_cast<u8>(score / 100);
            tenText->colIndex = static_cast<u8>((score % 100) / 10);
            oneText->colIndex = score % 10;
        }

        if (score < 10)
        {
            ECSSetComponentActive(m_tenNumber, typeid(Texture), FALSE);

            oneGeo->x = geo->x;
            oneText->colIndex = score;
        }
        else
        {
            ECSSetComponentActive(m_tenNumber, typeid(Texture), TRUE);
        }

        if (score < 100 && score >= 10)
        {
            tenGeo->x = geo->x;
            oneGeo->x = geo->x + tenGeo->width + SCORE_DIGIT_GAP;

            oneText->colIndex = score % 10;
            tenText->colIndex = static_cast<u8>(score / 10);
        }
    }

private:
    entity_id_t m_hunderedNumber;
    entity_id_t m_tenNumber;
    entity_id_t m_oneNumber;
};

SCRIPT_DEFINE(ScoreBoard)