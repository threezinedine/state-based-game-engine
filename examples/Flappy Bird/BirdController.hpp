#pragma once
#include <NTTEngine/NTTEngine.hpp>
#include "game_data.hpp"

class BirdController : public Script
{
public:
    void OnCreate() override;
    void OnUpdate(f32 delta) override;

    void OnCollide(List<entity_id_t> others);
};