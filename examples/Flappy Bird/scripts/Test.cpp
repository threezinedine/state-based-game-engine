#include <NTTEngine/NTTEngine.hpp>

class TestController : public Script
{
protected:
    void OnEnterImpl() override
    {
        // NTT_APP_DEBUG("The file is loaded");
    }

    void OnUpdateImpl(f32 delta) override
    {
    }

    void OnExitImpl() override
    {
        // NTT_APP_DEBUG("The file is unloaded");
    }
};

SCRIPT_DEFINE

void *CreateInstance(void *data)
{
    return new TestController();
}

void DeleteInstance(void *obj)
{
    delete static_cast<TestController *>(obj);
}