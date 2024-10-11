#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/editor/editor_log_handler.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include "imgui.h"
#include "rlImGui.h"
#include "log_window.hpp"

namespace ntt
{
    using namespace log;

    namespace
    {
        ImGuiTextBuffer s_buf;
        ImGuiTextFilter s_filter;
        ImVector<i32> s_lineOffsets;

        b8 s_autoScroll = TRUE;
    } // namespace

    class LogWindow::Impl
    {
    public:
    };

    LogWindow::LogWindow()
        : OpenClosableWindow("Log"), m_impl(CreateScope<Impl>())
    {
    }

    LogWindow::~LogWindow()
    {
    }

    void LogWindow::InitImpl()
    {
        s_buf.clear();
        s_lineOffsets.clear();
        s_lineOffsets.push_back(0);
    }

    void LogWindow::UpdateImpl(b8 *p_open)
    {
        if (ImGui::Begin("Log", p_open))
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
                {
                    s_buf.clear();
                    s_lineOffsets.clear();
                    s_lineOffsets.push_back(0);
                }
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
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void LogWindow::ShutdownImpl()
    {
        s_buf.clear();
        s_lineOffsets.clear();
    }

    void AppendLog(const char *log)
    {
        auto oldSize = s_buf.size();
        s_buf.append(log);
        s_lineOffsets.push_back(oldSize);
    }
} // namespace ntt
