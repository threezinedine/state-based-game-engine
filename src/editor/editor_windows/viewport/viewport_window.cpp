#include "viewport_window.hpp"
#include <NTTEngine/structures/size.hpp>
#include "imgui.h"
#include "rlImGui.h"
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>

namespace ntt
{
    using namespace log;

    class ViewportWindow::Impl
    {
    public:
        RenderTexture renderTexture;

        // The ratio between the viewport and the actual game screen. It is used
        //      for transforming the mouse position from the viewport to the game screen
        f32 viewPortRatio = 1.0f;

        f32 viewPortOffsetX = 0.0f;
        f32 viewPortOffsetY = 0.0f;
        f32 viewPortSizeX = 0.0f;
        f32 viewPortSizeY = 0.0f;

        f32 screenWidth = 800;
        f32 screenHeight = 600;

        Position EditorToViewportPosTransform(const Position &pos)
        {
            return {(pos.x - viewPortOffsetX) / viewPortRatio,
                    (pos.y - viewPortOffsetY) / viewPortRatio};
        }

        Position GameToViewportPosTransform(const Position &pos)
        {

            f32 transformedPosX = pos.x / screenWidth * viewPortSizeX;
            f32 transformedPosY = pos.y / screenHeight * viewPortSizeY;
            return {transformedPosX, transformedPosY};
        }

        Size GameToViewportSizeTransform(const Size &size)
        {
            return {size.width / screenWidth * viewPortSizeX,
                    size.height / screenHeight * viewPortSizeY};
        }
    };

    ViewportWindow::ViewportWindow(f32 width, f32 height)
        : m_impl(CreateScope<Impl>())
    {
        m_impl->renderTexture = LoadRenderTexture(width, height);
        input::SetMousePositionTransformCallback(
            std::bind(&Impl::EditorToViewportPosTransform, m_impl.get(), std::placeholders::_1));

        m_impl->screenHeight = height;
        m_impl->screenWidth = width;
    }

    ViewportWindow::~ViewportWindow()
    {
    }

    void ViewportWindow::OnStartDraw()
    {
        BeginTextureMode(m_impl->renderTexture);
    }

    void ViewportWindow::OnStopDraw()
    {
        EndTextureMode();
    }

    void ViewportWindow::Init()
    {
    }

    void ViewportWindow::Update()
    {
        ImGui::Begin("Viewport");
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 viewportSize = {0, 0};

        b8 isFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        input::SetInputModuleState(isFocus);

        f32 aspectRatio = static_cast<f32>(m_impl->screenWidth) /
                          static_cast<f32>(m_impl->screenHeight);

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

        m_impl->viewPortRatio = viewportSize.x / m_impl->screenWidth;
        ImVec2 windowPos = ImGui::GetWindowPos();

        ImGui::SetCursorPosX(size.x / 2 - viewportSize.x / 2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + size.y / 2 - viewportSize.y / 2);

        m_impl->viewPortOffsetX = windowPos.x + ImGui::GetCursorPosX();
        m_impl->viewPortOffsetY = windowPos.y + ImGui::GetCursorPosY();
        m_impl->viewPortSizeX = viewportSize.x;
        m_impl->viewPortSizeY = viewportSize.y;

        rlImGuiImageRect(
            &m_impl->renderTexture.texture,
            viewportSize.x,
            viewportSize.y,
            {0, 0,
             static_cast<float>(m_impl->renderTexture.texture.width),
             -static_cast<float>(m_impl->renderTexture.texture.height)});
        ImGui::End();
    }

    void ViewportWindow::Shutdown()
    {
    }
} // namespace ntt
