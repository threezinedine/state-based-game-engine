#pragma once
#include <NTTEngine/NTTEngine.hpp>

class BirdController : public Script
{
protected:
    void OnCreateImpl() override;
    void OnUpdateImpl(f32 delta) override;

    void OnCollide(List<entity_id_t> others);

    void StopBird();
    void StartBird();

private:
    b8 m_isPlaying = FALSE;
};