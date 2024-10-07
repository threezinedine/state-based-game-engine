#include "editor_viewport_window.hpp"
#include <NTTEngine/structures/size.hpp>
#include "imgui.h"
#include "rlImGui.h"
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>

namespace ntt
{
    using namespace log;

    namespace
    {
        RenderTexture s_renderTexture;
        // The ratio between the viewport and the actual game screen. It is used
        //      for transforming the mouse position from the viewport to the game screen
        f32 s_viewPortRatio = 1.0f;

        f32 s_viewPortOffsetX = 0.0f;
        f32 s_viewPortOffsetY = 0.0f;
        f32 s_viewPortSizeX = 0.0f;
        f32 s_viewPortSizeY = 0.0f;

        f32 s_screenWidth = 800;
        f32 s_screenHeight = 600;

        Position EditorToViewportPosTransform(const Position &pos)
        {
            return {(pos.x - s_viewPortOffsetX) / s_viewPortRatio,
                    (pos.y - s_viewPortOffsetY) / s_viewPortRatio};
        }

        Position GameToViewportPosTransform(const Position &pos)
        {
            if (pos.x == 475 && pos.y == 30)
            {
                NTT_ENGINE_DEBUG("The pos: ({}, {})", pos.x, pos.y);
            }

            f32 transformedPosX = pos.x / s_screenWidth * s_viewPortSizeX;
            f32 transformedPosY = pos.y / s_screenHeight * s_viewPortSizeY;
            return {transformedPosX, transformedPosY};
        }

        Size GameToViewportSizeTransform(const Size &size)
        {
            return {size.width / s_screenWidth * s_viewPortSizeX,
                    size.height / s_screenHeight * s_viewPortSizeY};
        }
    }

    void EditorViewportWindowInit(f32 width, f32 height, List<String> sceneNames)
    {
        s_renderTexture = LoadRenderTexture(width, height);
        input::SetMousePositionTransformCallback(EditorToViewportPosTransform);

        s_screenHeight = height;
        s_screenWidth = width;
    }

    void EditorViewportWindowStartDraw()
    {
        BeginTextureMode(s_renderTexture);
    }

    void EditorViewportWindowUpdate(b8 *p_open, b8 isRunning)
    {
        ImGui::Begin("Viewport");
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 viewportSize = {0, 0};

        b8 isFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        input::SetInputModuleState(isFocus);

        f32 aspectRatio = static_cast<f32>(s_screenWidth) /
                          static_cast<f32>(s_screenHeight);

        if (aspectRatio * size.y > size.x)
        {
            viewportSize.x = size.x;
            viewportSize.y = size.x / aspectRatio;
        }
        else
        {
            viewportSize.x = size.y * aspectRatio;
            viewportSize.y = size.y;
        }

        s_viewPortRatio = viewportSize.x / s_screenWidth;
        ImVec2 windowPos = ImGui::GetWindowPos();

        ImGui::SetCursorPosX(size.x / 2 - viewportSize.x / 2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + size.y / 2 - viewportSize.y / 2);

        s_viewPortOffsetX = windowPos.x + ImGui::GetCursorPosX();
        s_viewPortOffsetY = windowPos.y + ImGui::GetCursorPosY();
        s_viewPortSizeX = viewportSize.x;
        s_viewPortSizeY = viewportSize.y;

        rlImGuiImageRect(
            &s_renderTexture.texture,
            viewportSize.x,
            viewportSize.y,
            {0, 0,
             static_cast<float>(s_renderTexture.texture.width),
             -static_cast<float>(s_renderTexture.texture.height)});
        ImGui::End();
    }

    void EditorViewportWindowEndDraw()
    {
        EndTextureMode();
    }

    void EditorViewportWindowShutdown()
    {
    }

} // namespace ntt
