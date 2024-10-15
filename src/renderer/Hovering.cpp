#include <NTTEngine/renderer/Hovering.hpp>
#include "imgui.h"

namespace ntt::renderer
{
    String Hovering::GetName() const
    {
        return "Hovering";
    }

    JSON Hovering::ToJSON() const
    {
        JSON json("{}");
        return json;
    }

    void Hovering::FromJSON(const JSON &json)
    {
    }

    void Hovering::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
    }
} // namespace ntt
