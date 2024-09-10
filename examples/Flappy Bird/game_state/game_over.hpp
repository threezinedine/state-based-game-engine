#pragma once
#include <NTTEngine/NTTEngine.hpp>

class GameOver : public State
{
protected:
    void OnEnterImpl() override;
    String OnNavigateImpl() override;
    void OnUpdateImpl(f32 delta) override;
    void OnExitImpl() override;

private:
    entity_id_t m_gameOverMessage = INVALID_ENTITY_ID;
};