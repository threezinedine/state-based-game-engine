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
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 - 50, 50, 20, 20, 0, PRIORITY_1),
                ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("numbers"), 0, 0),
            });

        m_tenNumber = ECSCreateEntity(
            "ten",
            {
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, 50, 20, 20, 0, PRIORITY_1),
                ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("numbers"), 0, 0),
            });

        m_oneNumber = ECSCreateEntity(
            "one",
            {
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 + 50, 50, 20, 20, 0, PRIORITY_1),
                ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("numbers"), 0, 0),
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

        auto hunderedText = ECS_GET_COMPONENT(m_hunderedNumber, TextureComponent);
        auto tenText = ECS_GET_COMPONENT(m_tenNumber, TextureComponent);
        auto oneText = ECS_GET_COMPONENT(m_oneNumber, TextureComponent);

        hundredGeo->pos.y = geo->pos.y;
        tenGeo->pos.y = geo->pos.y;
        oneGeo->pos.y = geo->pos.y;

        if (score < 100)
        {
            ECSSetComponentActive(m_hunderedNumber, typeid(Geometry), FALSE);
        }
        else
        {
            ECSSetComponentActive(m_hunderedNumber, typeid(Geometry), TRUE);

            hundredGeo->pos.x = geo->pos.x;
            tenGeo->pos.x = geo->pos.x + hundredGeo->size.width + SCORE_DIGIT_GAP;
            oneGeo->pos.x = geo->pos.x + hundredGeo->size.width + tenGeo->size.width + SCORE_DIGIT_GAP * 2;

            hunderedText->colIndex = static_cast<u8>(score / 100);
            tenText->colIndex = static_cast<u8>((score % 100) / 10);
            oneText->colIndex = score % 10;
        }

        if (score < 10)
        {
            ECSSetComponentActive(m_tenNumber, typeid(Geometry), FALSE);

            oneGeo->pos.x = geo->pos.x;
            oneText->colIndex = score;
        }
        else
        {
            ECSSetComponentActive(m_tenNumber, typeid(Geometry), TRUE);
        }

        if (score < 100 && score >= 10)
        {
            tenGeo->pos.x = geo->pos.x;
            oneGeo->pos.x = geo->pos.x + tenGeo->size.width + SCORE_DIGIT_GAP;

            oneText->colIndex = score % 10;
            tenText->colIndex = static_cast<u8>(score / 10);
        }
    }

private:
    entity_id_t m_hunderedNumber;
    entity_id_t m_tenNumber;
    entity_id_t m_oneNumber;
};

SCRIPT_DEFINE(ScoreBoard, Script)