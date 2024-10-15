#include "image_window.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/editor/editor_clipboard.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    using namespace log;
    using namespace event;

    class ImageWindow::Impl
    {
    public:
        String file;

        RenderTexture renderTexture;
        Texture texture;

        u8 numRow = 1;
        u8 numCol = 1;

        // The ratio between the viewport and the actual game screen. It is used
        //      for transforming the mouse position from the viewport to the game screen
        f32 viewPortRatio = 1.0f;

        f32 viewPortOffsetX = 0.0f;
        f32 viewPortOffsetY = 0.0f;
        f32 viewPortSizeX = 0.0f;
        f32 viewPortSizeY = 0.0f;
    };

    ImageWindow::ImageWindow(const String &file, u8 numRow, u8 numCol)
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
        ChangeImage(file, numRow, numCol);
    }

    ImageWindow::~ImageWindow()
    {
        PROFILE_FUNCTION();
    }

    void ImageWindow::ChangeImage(const String &file, u8 numRow, u8 numCol)
    {
        PROFILE_FUNCTION();
        m_impl->file = file;

        if (m_impl->texture.id != 0)
        {
            UnloadTexture(m_impl->texture);
        }
        m_impl->texture = LoadTexture(m_impl->file.RawString().c_str());
        if (m_impl->texture.id == 0)
        {
            NTT_ENGINE_DEBUG("Cannot load {}", GetFileName(m_impl->file));
            return;
        }

        if (m_impl->renderTexture.texture.id != 0)
        {
            UnloadRenderTexture(m_impl->renderTexture);
        }
        m_impl->renderTexture = LoadRenderTexture(m_impl->texture.width, m_impl->texture.height);
        if (m_impl->renderTexture.texture.id == 0)
        {
            NTT_ENGINE_DEBUG("Cannot load render texture for {}", GetFileName(m_impl->file));
            return;
        }

        ChangeGrid(numRow, numCol);
    }

    void ImageWindow::ChangeGrid(u8 numRow, u8 numCol)
    {
        PROFILE_FUNCTION();
        m_impl->numCol = numCol;
        m_impl->numRow = numRow;

        if (m_impl->renderTexture.id == 0)
        {
            return;
        }

        BeginTextureMode(m_impl->renderTexture);

        f32 width = static_cast<f32>(m_impl->texture.width);
        f32 height = static_cast<f32>(m_impl->texture.height);

        ClearBackground(BLACK);
        DrawTexturePro(
            m_impl->texture,
            {0, 0, width, height},
            {width / 2, height / 2, width, height},
            {width / 2, height / 2},
            0,
            WHITE);

        // Draw grid lines
        Color gridColor = RED; // Set the color for the grid lines

        // Draw vertical lines
        for (u8 col = 1; col < numCol; ++col)
        {
            float x = col * (width / numCol);
            DrawLine(x, 0, x, height, gridColor);
        }

        // Draw horizontal lines
        for (u8 row = 1; row < numRow; ++row)
        {
            float y = row * (height / numRow);
            DrawLine(0, y, width, y, gridColor);
        }

        EndTextureMode();
    }

    void ImageWindow::InitImpl()
    {
        PROFILE_FUNCTION();
    }

    void ImageWindow::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        PROFILE_FUNCTION();
        if (m_impl->renderTexture.id == 0)
        {
            return;
        }

        if (ImGui::Begin("Image Resource", p_open))
        {
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImVec2 viewportSize = {0, 0};

            b8 isFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            f32 aspectRatio = static_cast<f32>(m_impl->renderTexture.texture.width) /
                              static_cast<f32>(m_impl->renderTexture.texture.height);

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

            m_impl->viewPortRatio = viewportSize.x / m_impl->renderTexture.texture.width;
            ImVec2 windowPos = ImGui::GetWindowPos();

            ImGui::SetCursorPosX(size.x / 2 - viewportSize.x / 2);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + size.y / 2 - viewportSize.y / 2);

            m_impl->viewPortOffsetX = windowPos.x + ImGui::GetCursorPosX();
            m_impl->viewPortOffsetY = windowPos.y + ImGui::GetCursorPosY();
            m_impl->viewPortSizeX = viewportSize.x;
            m_impl->viewPortSizeY = viewportSize.y;

            f32 width = static_cast<f32>(m_impl->renderTexture.texture.width);
            f32 height = static_cast<f32>(m_impl->renderTexture.texture.height);

            // Get the mouse position
            ImVec2 mousePos = ImGui::GetMousePos();

            BeginTextureMode(m_impl->renderTexture);
            ClearBackground(BLACK);
            DrawTexturePro(
                m_impl->texture,
                {0, 0, width, height},
                {width / 2, height / 2, width, height},
                {width / 2, height / 2},
                0,
                WHITE);

            Color gridColor = RED; // Set the color for the grid lines
            for (u8 col = 1; col < m_impl->numCol; ++col)
            {
                float x = col * (width / m_impl->numCol);
                DrawLine(x, 0, x, height, gridColor);
            }
            for (u8 row = 1; row < m_impl->numRow; ++row)
            {
                float y = row * (height / m_impl->numRow);
                DrawLine(0, y, width, y, gridColor);
            }

            // Calculate the grid cell dimensions
            f32 cellWidth = width / m_impl->numCol;
            f32 cellHeight = height / m_impl->numRow;

            f32 cellWidthViewport = cellWidth * m_impl->viewPortRatio;
            f32 cellHeightViewport = cellHeight * m_impl->viewPortRatio;

            // Determine the mouse position relative to the grid
            f32 mouseX = mousePos.x - m_impl->viewPortOffsetX;
            f32 mouseY = mousePos.y - m_impl->viewPortOffsetY;

            // Identify the specific grid cell based on the mouse position
            u8 hoveredCol = static_cast<u8>(mouseX / cellWidthViewport);
            u8 hoveredRow = static_cast<u8>(mouseY / cellHeightViewport);

            // Check if the mouse is within the grid bounds
            b8 isMouseWithinGrid = mouseX >= 0 && mouseX < viewportSize.x &&
                                   mouseY >= 0 && mouseY < viewportSize.y;

            if (isMouseWithinGrid)
            {
                // Highlight the hovered grid cell
                DrawRectangle(hoveredCol * cellWidth,
                              hoveredRow * cellHeight,
                              cellWidth, cellHeight,
                              Fade(RED, 0.5f));

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    EditorClipboard_ChooseGrid(hoveredRow, hoveredCol);
                    NTT_ENGINE_INFO("Grid cell selected: ({}, {})", hoveredRow, hoveredCol);
                }
            }
            EndTextureMode();

            rlImGuiImageRect(
                &m_impl->renderTexture.texture,
                viewportSize.x,
                viewportSize.y,
                {0, 0,
                 width,
                 -height});
        }

        ImGui::End();
    }

    void ImageWindow::ShutdownImpl()
    {
        PROFILE_FUNCTION();
        if (m_impl->texture.id != 0)
        {
            UnloadTexture(m_impl->texture);
        }

        if (m_impl->renderTexture.texture.id != 0)
        {
            UnloadRenderTexture(m_impl->renderTexture);
        }
    }
} // namespace ntt
