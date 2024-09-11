#pragma once
#include <NTTEngine/NTTEngine.hpp>

class ScoreBoard : public Script
{
public:
    void OnCreate();
    void OnUpdate(f32 deltaTime);
    void OnDestroy();

private:
    entity_id_t m_hunderedNumber;
    entity_id_t m_tenNumber;
    entity_id_t m_oneNumber;
};