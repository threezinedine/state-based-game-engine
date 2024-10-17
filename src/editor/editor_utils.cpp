#include <NTTEngine/editor/editor_utils.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    void Editor_ChangeScene(const String &scene_name)
    {
        String newScene = scene_name;

        EventContext context;
        context.u32_data[0] = newScene.Length() + 1;
        TriggerEvent(NTT_EDITOR_OPEN_SCENE, newScene.RawString().data(), context);
    }
} // namespace ntt
