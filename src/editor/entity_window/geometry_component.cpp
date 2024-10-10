#include "geometry_component.hpp"
#include "imgui.h"
#include <NTTEngine/platforms/application.hpp>

namespace ntt
{
    void GeometryComponentInit()
    {
    }

    void GeometryComponentDraw(Ref<Geometry> geo)
    {
        ImGui::Separator();
        auto windowSize = GetWindowSize();
        ImGui::SliderFloat2("Position", &geo->pos.x, 0.0f, GetWindowSize().width);
        ImGui::SliderFloat2("Size", &geo->size.width, 0.0f, GetWindowSize().height);
        ImGui::SliderFloat("Rotation", &geo->rotation, 0.0f, 360.0f);
    }

    void GeometryComponentShutdown()
    {
    }
} // namespace ntt
