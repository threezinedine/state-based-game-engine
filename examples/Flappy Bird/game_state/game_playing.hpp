#pragma once
#include <NTTEngine/NTTEngine.hpp>

class GamePlaying : public State
{
protected:
    void OnEnterImpl() override;
    String OnNavigateImpl() override;
};