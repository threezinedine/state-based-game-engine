#include <NTTEngine/NTTEngine.hpp>

class Bird : public Script
{
public:
public:
    Bird(void *data)
    {
    }

    ~Bird()
    {
    }

protected:
    void OnUpdateImpl(f32 deltaTime) override
    {
        auto mass = GetComponent<Mass>();
        auto texture = GetComponent<TextureComponent>();
        auto geo = GetComponent<Geometry>();

        if (mass == nullptr || texture == nullptr || geo == nullptr)
        {
            return;
        }

        if (mass->velocity_y < 0.0f)
        {
            geo->rotation -= 1.0f;
        }
        else
        {
            geo->rotation += 1.0f;
        }

        if (CHECK_PRESS(NTT_KEY_SPACE))
        {
            if (mass != nullptr)
            {
                mass->velocity_y = -2.0f;
            }
        }
    }

    void OnHoverEnter() override
    {
        NTT_ENGINE_INFO("Bird is hovered");
    }
};

SCRIPT_DEFINE(Bird, Script);