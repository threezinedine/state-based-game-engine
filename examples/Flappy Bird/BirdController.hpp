#pragma once
#include <NTTEngine/NTTEngine.hpp>

class BirdController : public Script
{
protected:
    void OnEnterImpl() override;
    void OnUpdateImpl(f32 delta) override;

    void OnCollide(List<entity_id_t> others);

    void StopBird();
    void StartBird();

    void OnExitImpl() override
    {
        NTT_APP_INFO("Yah, see you later!");
    }

    void OnHover(HoveringContext &context)
    {
        NTT_APP_INFO("Bird is hovering!");
    }

private:
    b8 m_isPlaying = FALSE;
};