#include "viewport_window.hpp"
#include <NTTEngine/structures/size.hpp>
#include "imgui.h"
#include "rlImGui.h"
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include "../editor_tool/editor_tool.hpp"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/renderer/Camera.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/platforms/application.hpp>

namespace ntt
{
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

        Scope<EditorTool> editorTool;

        Position mouseStartPos;
        Position cameraStartPos;
        camera_id_t cameraId;

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

        void ResetCamera(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            auto camera = GetCameraInfo(cameraId)->camera;
            camera->Reset();
        }
    };

    ViewportWindow::ViewportWindow(f32 width, f32 height)
        : m_impl(CreateScope<Impl>())
    {
        m_impl->renderTexture = LoadRenderTexture(width, height);
        SetMousePositionTransformCallback(
            std::bind(&Impl::EditorToViewportPosTransform, m_impl.get(), std::placeholders::_1));

        m_impl->screenHeight = height;
        m_impl->screenWidth = width;

        m_impl->editorTool = CreateScope<EditorTool>();

        RegisterEvent(NTT_EDITOR_RESET_CAMERA,
                      std::bind(&Impl::ResetCamera,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        m_impl->cameraId = AddCamera({0, 0}, {GetWindowSize().width, GetWindowSize().height});
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
        m_impl->editorTool->Init();
        m_impl->editorTool->Open();
    }

    void ViewportWindow::Update(ImGuiWindowFlags flags)
    {
        m_impl->editorTool->Update(flags);

        ImGui::Begin("Viewport");
        b8 isFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        SetInputModuleState(isFocus);
        auto camera = GetCameraInfo(m_impl->cameraId)->camera;
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 windowPos = ImGui::GetWindowPos();

        b8 isMouseInside = ImGui::IsMouseHoveringRect(
            windowPos,
            {windowPos.x + windowSize.x, windowPos.y + windowSize.y});

        if (GetMouseScroll() != 0 && isFocus && isMouseInside)
        {
            auto currentMouse = GetMousePosition();
            auto windowSize = GetWindowSize();

            camera->camZoom += 0.03f * GetMouseScroll();

            if (camera->camZoom < 0.3f)
            {
                camera->camZoom = 0.3f;
            }

            if (camera->camZoom > 3.0f)
            {
                camera->camZoom = 3.0f;
            }

            camera->ShiftCamera(
                {camera->ReverseTransformX(currentMouse.x),
                 camera->ReverseTransformY(currentMouse.y)},
                currentMouse);
        }

        if (CheckState(NTT_BUTTON_RIGHT, NTT_PRESS) && isFocus && isMouseInside)
        {
            m_impl->mouseStartPos = GetMousePosition();
            m_impl->cameraStartPos = camera->camPos;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && isFocus && isMouseInside)
        {
            Position movedMouse = GetMousePosition();
            Position delta = {movedMouse.x - m_impl->mouseStartPos.x,
                              movedMouse.y - m_impl->mouseStartPos.y};

            camera->camPos.x = delta.x / camera->camZoom + m_impl->cameraStartPos.x;
            camera->camPos.y = delta.y / camera->camZoom + m_impl->cameraStartPos.y;
        }

        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 viewportSize = {0, 0};

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
        m_impl->editorTool->Shutdown();
    }
} // namespace ntt
