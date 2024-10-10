#include "mass_component.hpp"
#include "imgui.h"

namespace ntt
{
    void MassComponentInit()
    {
    }

    void MassComponentDraw(Ref<Mass> mass)
    {
        ImGui::Separator();
        ImGui::SliderFloat("Mass", &mass->mass, 1.0f, 100.0f);
    }

    void MassComponentShutdown()
    {
    }

} // namespace ntt
