#include "geometry_component.hpp"
#include "imgui.h"
#include <NTTEngine/platforms/application.hpp>

namespace ntt
{
    void GeometryComponentDraw(Ref<Geometry> geo)
    {
        auto windowSize = GetWindowSize();
        ImGui::SliderFloat2("Position", &geo->x, 0.0f, GetWindowSize().width);
        ImGui::SliderFloat2("Size", &geo->width, 0.0f, GetWindowSize().height);
    }
} // namespace ntt
