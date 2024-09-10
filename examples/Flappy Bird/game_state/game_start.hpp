#pragma once
#include <NTTEngine/NTTEngine.hpp>

class GameStart : public State
{
protected:
    void OnEnterImpl() override;
    String OnNavigateImpl() override;
    void OnUpdateImpl(f32 delta) override;
    void OnExitImpl() override;

private:
    entity_id_t m_messageEntity = INVALID_ENTITY_ID;
};
