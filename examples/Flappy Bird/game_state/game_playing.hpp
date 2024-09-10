#pragma once
#include <NTTEngine/NTTEngine.hpp>

class GamePlaying : public State
{
protected:
    String OnNavigateImpl() override;
    void OnUpdateImpl(f32 delta) override;
};