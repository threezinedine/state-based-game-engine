#pragma once
#include <NTTEngine/NTTEngine.hpp>

class ScoreBoard : public Script
{
protected:
    void OnEnterImpl();
    void OnExitImpl();
    void OnScoreChanged(u16 score);

private:
    entity_id_t m_hunderedNumber;
    entity_id_t m_tenNumber;
    entity_id_t m_oneNumber;
};