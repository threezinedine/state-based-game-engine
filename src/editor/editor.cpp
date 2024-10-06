#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include "rlImGui.h"
#include "imgui.h"
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/structures/size.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>

namespace ntt
{
    using namespace log;
    using namespace event;

    namespace
    {
        b8 s_useEditor = FALSE;
        b8 s_isRunning = FALSE;

        b8 s_shouldStop = FALSE;
        b8 s_shouldStart = FALSE;

        f32 s_screenWidth = 800;
        f32 s_screenHeight = 600;

        b8 s_openLog = FALSE;

        RenderTexture s_renderTexture;
        ImGuiTextBuffer s_buf;
        ImGuiTextFilter s_filter;
        ImVector<i32> s_lineOffsets;
        b8 s_autoScroll = TRUE;

        // The ratio between the viewport and the actual game screen. It is used
        //      for transforming the mouse position from the viewport to the game screen
        f32 s_viewPortRatio = 1.0f;

        f32 s_viewPortOffsetX = 0.0f;
        f32 s_viewPortOffsetY = 0.0f;
        f32 s_viewPortSizeX = 0.0f;
        f32 s_viewPortSizeY = 0.0f;

        void Clear()
        {
            s_buf.clear();
            s_lineOffsets.clear();
            s_lineOffsets.push_back(0);
        }

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

    void EditorInit(b8 use, u16 width, u16 height)
    {
        s_useEditor = use;

        if (s_useEditor)
        {
            TriggerEvent(NTT_EDITOR_STOP, {});
        }

        s_screenWidth = static_cast<f32>(width);
        s_screenHeight = static_cast<f32>(height);

        rlImGuiSetup(true);
        s_renderTexture = LoadRenderTexture(width, height);
        s_openLog = FALSE;

        input::SetMousePositionTransformCallback(EditorToViewportPosTransform);
    }

    void EditorRun()
    {
        if (!s_useEditor)
        {
            return;
        }

        if (s_isRunning)
        {
            return;
        }

        s_shouldStart = TRUE;
        s_isRunning = TRUE;
    }

    void EditorStop()
    {
        if (!s_useEditor)
        {
            return;
        }

        if (!s_isRunning)
        {
            return;
        }

        s_shouldStop = TRUE;
        s_isRunning = FALSE;
    }

    void EditorUpdate(f32 delta)
    {
        if (!s_useEditor)
        {
            return;
        }

        if (s_shouldStart)
        {
            TriggerEvent(NTT_EDITOR_START, {});
        }

        if (s_shouldStop)
        {
            TriggerEvent(NTT_EDITOR_STOP, {});
        }

        s_shouldStart = FALSE;
        s_shouldStop = FALSE;

        rlImGuiBegin();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Log", NULL, &s_openLog);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

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

        static b8 show = TRUE;
        if (show)
        {
            ImGui::ShowDemoWindow(&show);
        }

        if (s_openLog)
        {
            if (ImGui::Begin("Log", &s_openLog))
            {
                if (ImGui::BeginPopup("Options"))
                {
                    ImGui::Checkbox("Auto-scroll", &s_autoScroll);
                    ImGui::EndPopup();
                }

                if (ImGui::Button("Options"))
                {
                    ImGui::OpenPopup("Options");
                }

                ImGui::SameLine();
                b8 clear = ImGui::Button("Clear");
                ImGui::SameLine();
                b8 copy = ImGui::Button("Copy");
                ImGui::SameLine();
                s_filter.Draw("Filter", -100.0f);

                ImGui::Separator();

                if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
                {
                    if (clear)
                        Clear();
                    if (copy)
                        ImGui::LogToClipboard();

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                    const char *buf = s_buf.begin();
                    const char *buf_end = s_buf.end();
                    if (s_filter.IsActive())
                    {
                        for (int line_no = 0; line_no < s_lineOffsets.Size; line_no++)
                        {
                            const char *line_start = buf + s_lineOffsets[line_no];
                            const char *line_end = (line_no + 1 < s_lineOffsets.Size) ? (buf + s_lineOffsets[line_no + 1] - 1) : buf_end;
                            if (s_filter.PassFilter(line_start, line_end))
                                ImGui::TextUnformatted(line_start, line_end);
                        }
                    }
                    else
                    {
                        ImGuiListClipper clipper;
                        clipper.Begin(s_lineOffsets.Size);
                        while (clipper.Step())
                        {
                            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                            {
                                const char *line_start = buf + s_lineOffsets[line_no];
                                const char *line_end = (line_no + 1 < s_lineOffsets.Size) ? (buf + s_lineOffsets[line_no + 1] - 1) : buf_end;
                                ImGui::TextUnformatted(line_start, line_end);
                            }
                        }
                        clipper.End();
                    }
                    ImGui::PopStyleVar();

                    if (s_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    {
                        ImGui::SetScrollHereY(1.0f);
                    }
                }
            }
            ImGui::EndChild();
            ImGui::End();
        }

        rlImGuiEnd();
    }

    void AppendLog(const char *log)
    {
        auto oldSize = s_buf.size();

        s_buf.append(log);

        s_lineOffsets.push_back(oldSize);
    }

    void EditorBeginDraw()
    {
        if (!s_useEditor)
        {
            return;
        }

        BeginTextureMode(s_renderTexture);
    }

    void EditorEndDraw()
    {
        if (!s_useEditor)
        {
            return;
        }

        EndTextureMode();
    }

    void EditorShutdown()
    {
        NTT_ENGINE_INFO("The editor mode is shutdown");
    }
} // namespace ntt
