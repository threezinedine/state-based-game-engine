#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include "rlImGui.h"
#include "imgui.h"

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

        void Clear()
        {
            s_buf.clear();
            s_lineOffsets.clear();
            s_lineOffsets.push_back(0);
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

        ImGui::SetCursorPosX(0);
        ImGui::SetCursorPosX(size.x / 2 - viewportSize.x / 2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (size.y / 2 - viewportSize.y / 2));

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
                    ImGui::EndChild();
                }
                ImGui::End();
            }
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
