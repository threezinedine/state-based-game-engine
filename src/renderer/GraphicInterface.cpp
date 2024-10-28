#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <cstring>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/core/object.hpp>

#include "Raylib_GraphicAPI.hpp"
#include "Fake_GraphicAPI.hpp"

namespace ntt
{
#define TEXTURE_MAX 1000
#define TOOL_TIP_FONT_SIZE 10
#define TOOL_TIP_PADDING 5
#define TOOL_TOP_OFFSET_X 20
#define TOOL_TOP_OFFSET_Y 20
#define MAX_PRIORITIES (LAYER_PRIORITY_RANGE * MAX_LAYERS)

    /**
     * All the needed information for rendering the texture
     */
    struct TextureInfo : public Object
    {
        Texture2D texture; ///< The texture which is loaded
        Grid grid;         ///< The grid of the texture
        String path;       ///< The path of the texture
        f32 frameWith;     ///< The width each frame in the texture
        f32 frameHeight;   ///< The height each frame in the texture

        TextureInfo(Texture2D texture, const String &path) : texture(texture), path(path) {}
        TextureInfo(Texture2D texture, const Grid &grid, const String &path)
            : texture(texture), grid(grid), path(path) {}
    };

    struct DrawInfo
    {
        entity_id_t entity_id;
        resource_id_t texture_id;
        f32 fromX;
        f32 fromY;
        f32 fromWidth;
        f32 fromHeight;
        f32 toX;
        f32 toY;
        f32 toWidth;
        f32 toHeight;
        f32 rotate;
        String tooltip;
        b8 drawText = FALSE;
        String text;
        u32 fontSize;
        RGBAColor color;
        b8 drawLine = FALSE;
        u8 lineType;
        f32 toXEnd;
        f32 toYEnd;
    };

    namespace
    {
        Scope<Store<resource_id_t, TextureInfo>> s_textureStore;

        // List<DrawInfo> s_drawList;
        // List<DrawInfo> s_priorityDrawList;

        // Store all the priorities draw with maximum 256 priorities
        // The higher priority will be drawn on the top of the lower priority
        // It should be cleared after each frame
        Scope<List<DrawInfo>> s_drawLists[MAX_PRIORITIES];

        // Stack of all texture ID which is hovered by the mouse
        // It also be cleared after each frame
        // The higher priority texture will be on the top of the stack
        // If the same priority, the last hovered texture will be on the top
        List<entity_id_t> s_hoveredTextures;

        Scope<GraphicAPI> s_graphicAPI;

        Scope<Store<camera_id_t, CameraInfo>> s_cameraStore;

        b8 s_test = FALSE;
    } // namespace

    void RendererInit(b8 test)
    {
        PROFILE_FUNCTION();

        s_textureStore = CreateScope<Store<resource_id_t, TextureInfo>>(
            RESOURCE_ID_DEFAULT,
            TEXTURE_MAX,
            [](Ref<TextureInfo> texture, Ref<TextureInfo> other) -> b8
            { return texture->path == other->path; });

        s_cameraStore = CreateScope<Store<camera_id_t, CameraInfo>>(
            0,
            10,
            [](Ref<CameraInfo> camera, Ref<CameraInfo> other) -> b8
            { return FALSE; });

        if (test)
        {
            s_graphicAPI = CreateScope<FakeGraphicAPI>();
        }
        else
        {
            s_graphicAPI = CreateScope<RaylibGraphicAPI>();
        }
        s_test = test;

        memset(s_drawLists, 0, sizeof(s_drawLists));
    }

    resource_id_t LoadTexture(const String &path, const Grid &grid)
    {
        PROFILE_FUNCTION();

        if (!s_test)
        {
            auto existedTexturePaths = s_textureStore->GetByField<String>(
                path,
                [](Ref<TextureInfo> texture) -> String
                { return texture->path; });

            if (existedTexturePaths.size() > 0)
            {
                NTT_ENGINE_WARN("The texture is already loaded",
                                GetFileName(path, true));
                return RESOURCE_ID_DEFAULT;
            }

            if (IsExist(path) == FALSE)
            {
                NTT_ENGINE_WARN("The texture is not found: {}", GetFileName(path, true));
                return RESOURCE_ID_DEFAULT;
            }
        }

        auto texture = s_graphicAPI->LoadTexture(path);

        if (s_graphicAPI->IsLoadedSuccess(texture) != TRUE)
        {
            NTT_ENGINE_WARN("Loading the texture {} error", GetFileName(path, true));
            return RESOURCE_ID_DEFAULT;
        }

        auto textureInfo = CreateRef<TextureInfo>(texture, grid, path);

        textureInfo->frameWith = static_cast<f32>(texture.width) / grid.col;
        textureInfo->frameHeight = static_cast<f32>(texture.height) / grid.row;
        return s_textureStore->Add(textureInfo);
    }

    std::pair<Size, Size> ValidateSize(resource_id_t texture_id,
                                       const RectContext &context)
    {
        PROFILE_FUNCTION();
        auto textureInfo = s_textureStore->Get(texture_id);
        if (textureInfo == nullptr)
        {
            return {};
        }

        auto texture = textureInfo->texture;
        f32 textureWidth = static_cast<f32>(textureInfo->texture.width);
        f32 textureHeight = static_cast<f32>(textureInfo->texture.height);
        f32 frameWidth = textureInfo->frameWith;
        f32 frameHeight = textureInfo->frameHeight;
        f32 width = 0.0f;
        f32 height = 0.0f;

        if (context.size.width != SIZE_DEFAULT && context.size.height != SIZE_DEFAULT)
        {
            width = static_cast<f32>(context.size.width);
            height = static_cast<f32>(context.size.height);
        }
        else if (context.size.width != SIZE_DEFAULT)
        {
            width = static_cast<f32>(context.size.width);
            height = (f32)(frameHeight * context.size.width / frameWidth);
        }
        else if (context.size.height != SIZE_DEFAULT)
        {
            width = (f32)(frameWidth * context.size.height / frameHeight);
            height = static_cast<f32>(context.size.height);
        }
        else
        {
            width = static_cast<f32>(frameWidth);
            height = static_cast<f32>(frameHeight);
        }

        return {{static_cast<ntt_size_t>(width), static_cast<ntt_size_t>(height)},
                {static_cast<ntt_size_t>(textureWidth), static_cast<ntt_size_t>(textureHeight)}};
    }

    const Grid GetTextureGrid(resource_id_t texture_id)
    {
        PROFILE_FUNCTION();

        if (!s_textureStore->Contains(texture_id))
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not found", texture_id);
            return {1, 1};
        }

        auto textureInfo = s_textureStore->Get(texture_id);
        if (textureInfo == nullptr)
        {
            return {1, 1};
        }

        return textureInfo->grid;
    }

    String GetTexturePath(resource_id_t texture_id)
    {
        PROFILE_FUNCTION();

        if (!s_textureStore->Contains(texture_id))
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not found", texture_id);
            return "";
        }

        auto textureInfo = s_textureStore->Get(texture_id);
        if (textureInfo == nullptr)
        {
            return "";
        }

        return textureInfo->path;
    }

    void DrawTexture(resource_id_t texture_id,
                     const RectContext &context,
                     const Grid &cell,
                     const DrawContext &drawContext)
    {
        PROFILE_FUNCTION();

        if (texture_id == INVALID_RESOURCE_ID)
        {
            return;
        }

        if (!s_textureStore->Contains(texture_id))
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not found", texture_id);
            return;
        }

        auto textureInfo = s_textureStore->Get(texture_id);
        if (textureInfo == nullptr)
        {
            return;
        }

        auto grid = textureInfo->grid;
        Grid frame;
        frame.row = static_cast<u8>(cell.row) >= static_cast<u8>(grid.row)
                        ? static_cast<u8>(grid.row - 1)
                        : static_cast<u8>(cell.row);
        frame.col = static_cast<u8>(cell.col) >= static_cast<u8>(grid.col)
                        ? static_cast<u8>(grid.col - 1)
                        : static_cast<u8>(cell.col);
        auto texture = textureInfo->texture;
        f32 frameWidth = textureInfo->frameWith;
        f32 frameHeight = textureInfo->frameHeight;
        auto actualSize = ValidateSize(texture_id, context);

        if (drawContext.priority >= MAX_PRIORITIES)
        {
            NTT_ENGINE_WARN("The priority of the texture is out of range: {}",
                            static_cast<u32>(drawContext.priority));
            return;
        }

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        DrawInfo info;
        info.entity_id = drawContext.entity_id;
        info.texture_id = texture_id;
        info.fromX = frameWidth * frame.col;
        info.fromY = frameHeight * frame.row;
        info.fromWidth = frameWidth;
        info.fromHeight = frameHeight;
        info.toX = static_cast<f32>(context.position.x);
        info.toY = static_cast<f32>(context.position.y);
        info.toWidth = static_cast<f32>(actualSize.first.width);
        info.toHeight = static_cast<f32>(actualSize.first.height);
        info.rotate = static_cast<f32>(context.rotate);
        info.tooltip = drawContext.tooltip;
        info.drawText = FALSE;

        s_drawLists[drawContext.priority]->push_back(info);
    }

    void DrawText(const String &text,
                  const Position &position,
                  const DrawContext &drawContext)
    {
        PROFILE_FUNCTION();

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        DrawInfo info;
        info.entity_id = drawContext.entity_id;
        info.drawText = TRUE;
        info.text = text;
        info.toX = static_cast<f32>(position.x);
        info.toY = static_cast<f32>(position.y);
        info.fontSize = drawContext.fontSize;
        info.color = drawContext.color;

        s_drawLists[drawContext.priority]->push_back(info);
    }

    void DrawLine(const Position &start, const Position &end,
                  const DrawContext &drawContext)
    {
        PROFILE_FUNCTION();

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        DrawInfo info;
        info.drawLine = TRUE;
        info.entity_id = drawContext.entity_id;
        info.toX = static_cast<f32>(start.x);
        info.toY = static_cast<f32>(start.y);
        info.toXEnd = static_cast<f32>(end.x);
        info.toYEnd = static_cast<f32>(end.y);
        info.color = drawContext.color;
        info.lineType = drawContext.lineType;

        s_drawLists[drawContext.priority]->push_back(info);
    }

    void DrawRectangle(const RectContext &rect, const DrawContext &drawContext)
    {
        PROFILE_FUNCTION();

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        DrawInfo info;
        info.entity_id = drawContext.entity_id;
        info.drawText = FALSE;
        info.toX = static_cast<f32>(rect.position.x);
        info.toY = static_cast<f32>(rect.position.y);
        info.toWidth = static_cast<f32>(rect.size.width);
        info.toHeight = static_cast<f32>(rect.size.height);
        info.rotate = rect.rotate;
        info.tooltip = drawContext.tooltip;
        info.texture_id = INVALID_RESOURCE_ID;
        info.color = drawContext.color;

        s_drawLists[drawContext.priority]->push_back(info);
    }

    void GraphicUpdate()
    {
        PROFILE_FUNCTION();
        s_hoveredTextures.clear();

        auto mouse = GetMousePosition();

        auto highestPriority = MAX_PRIORITIES - 1;
        auto hoveredEntityId = INVALID_ENTITY_ID;
        RectContext context;
        context.position.x = 0;
        context.position.y = 0;
        context.size.width = 0;
        context.size.height = 0;
        context.rotate = 0;

        // TODO: can be optimzed for drawing by removing the iteration through
        //     the empty priorities

        for (highestPriority; highestPriority >= 0; highestPriority--)
        {
            if (s_drawLists[highestPriority] != nullptr)
            {
                break;
            }
        }

        auto s_availableCameras = s_cameraStore->GetAvailableIds();

        for (auto cameraId : s_availableCameras)
        {
            Ref<CameraInfo> cameraInfo = s_cameraStore->Get(cameraId);
            Ref<Camera> camera = cameraInfo->camera;

            position_t projectSpaceX = GetWindowSize().width / 2;
            position_t projectSpaceY = GetWindowSize().height / 2;
            ntt_size_t projectSpaceWidth = GetWindowSize().width;
            ntt_size_t projectSpaceHeight = GetWindowSize().height;

            Position transformedMouse = {
                camera->ReverseTransformX(mouse.x),
                camera->ReverseTransformY(mouse.y)};

            for (auto i = 0; i < MAX_PRIORITIES; i++)
            {
                if (s_drawLists[i] == nullptr)
                {
                    continue;
                }

                for (auto info : *s_drawLists[i])
                {
                    if (info.drawText)
                    {
                        s_graphicAPI->DrawText(
                            info.text,
                            camera->TransformX(info.toX),
                            camera->TransformY(info.toY),
                            camera->TransformWidth(info.fontSize),
                            info.color);
                    }
                    else if (info.drawLine)
                    {
                        s_graphicAPI->DrawLine(
                            camera->TransformX(info.toX),
                            camera->TransformY(info.toY),
                            camera->TransformX(info.toXEnd),
                            camera->TransformY(info.toYEnd),
                            info.color,
                            info.lineType);
                    }
                    else
                    {
                        if (info.texture_id == INVALID_RESOURCE_ID)
                        {
                            s_graphicAPI->DrawRectanglePro(
                                camera->TransformX(info.toX),
                                camera->TransformY(info.toY),
                                camera->TransformWidth(info.toWidth),
                                camera->TransformHeight(info.toHeight),
                                info.rotate,
                                info.color);
                        }
                        else
                        {
                            s_graphicAPI->DrawTexture(
                                s_textureStore->Get(info.texture_id)->texture,
                                info.fromX,
                                info.fromY,
                                info.fromWidth,
                                info.fromHeight,
                                camera->TransformX(info.toX),
                                camera->TransformY(info.toY),
                                camera->TransformWidth(info.toWidth),
                                camera->TransformHeight(info.toHeight),
                                info.rotate);
                        }

                        if (cameraInfo->hoverChecking == FALSE)
                        {
                            continue;
                        }

                        if (info.entity_id == INVALID_ENTITY_ID)
                        {
                            continue;
                        }

                        if (info.toX - info.toWidth / 2 <= transformedMouse.x &&
                            transformedMouse.x <= info.toX + info.toWidth / 2 &&
                            info.toY - info.toHeight / 2 <= transformedMouse.y &&
                            transformedMouse.y <= info.toY + info.toHeight / 2)
                        {
                            s_hoveredTextures.push_back(info.entity_id);

                            if (i < MAX_PRIORITIES - LAYER_PRIORITY_RANGE)
                            {
                                // store the highest priority hovered texture
                                hoveredEntityId = info.entity_id;
                                context.position.x = info.toX;
                                context.position.y = info.toY;
                                context.size.width = static_cast<ntt_size_t>(info.toWidth);
                                context.size.height = static_cast<ntt_size_t>(info.toHeight);
                                context.rotate = info.rotate;
                            }
                            else
                            {
                                hoveredEntityId = INVALID_ENTITY_ID;
                            }

                            if (info.tooltip != "" &&
                                i == highestPriority &&
                                i < MAX_PRIORITIES - LAYER_PRIORITY_RANGE)
                            {
                                auto windowSize = GetWindowSize();

                                auto textWidth = s_graphicAPI->GetTextWidth(
                                    info.tooltip,
                                    TOOL_TIP_FONT_SIZE);
                                auto textHeight = TOOL_TIP_FONT_SIZE;

                                auto toolTipX = transformedMouse.x + TOOL_TOP_OFFSET_X;

                                if (toolTipX + textWidth > windowSize.width)
                                {
                                    toolTipX -= (textWidth + TOOL_TIP_PADDING * 2 + TOOL_TOP_OFFSET_X);
                                }

                                auto toolTipY = transformedMouse.y + TOOL_TOP_OFFSET_Y;

                                if (toolTipY + textHeight > windowSize.height)
                                {
                                    toolTipY -= textHeight - TOOL_TIP_PADDING * 2 - TOOL_TOP_OFFSET_Y;
                                }

                                s_graphicAPI->DrawRectangle(
                                    toolTipX,
                                    toolTipY,
                                    textWidth + TOOL_TIP_PADDING * 2,
                                    textHeight + TOOL_TIP_PADDING * 2,
                                    {0, 255, 255, 255});

                                s_graphicAPI->DrawText(info.tooltip,
                                                       toolTipX + TOOL_TIP_PADDING,
                                                       toolTipY + TOOL_TIP_PADDING,
                                                       TOOL_TIP_FONT_SIZE, info.color);
                            }
                        }
                    }
                }

                s_drawLists[i]->clear();
                s_drawLists[i].reset();

                ASSERT(s_drawLists[i] == nullptr);
            }

            s_graphicAPI->DrawNoFillRectangle(
                camera->TransformX(0),
                camera->TransformY(0),
                camera->TransformWidth(projectSpaceWidth),
                camera->TransformHeight(projectSpaceHeight),
                {255, 255, 255, 255});
        }
    }

    camera_id_t AddCamera(Position outputFramePos, Size outputFrameSize)
    {
        PROFILE_FUNCTION();
        auto camera = CreateRef<Camera>(
            outputFramePos.x,
            outputFramePos.y,
            outputFrameSize.width,
            outputFrameSize.height);
        auto info = CreateRef<CameraInfo>();
        info->camera = camera;
        info->hoverChecking = TRUE;
        return s_cameraStore->Add(info);
    }

    Ref<CameraInfo> GetCameraInfo(camera_id_t cameraId)
    {
        PROFILE_FUNCTION();
        if (cameraId == INVALID_CAMERA_ID)
        {
            NTT_ENGINE_WARN("The camera ID is invalid");
            return nullptr;
        }

        if (!s_cameraStore->Contains(cameraId))
        {
            NTT_ENGINE_WARN("The camera with the ID {} is not found", cameraId);
            return nullptr;
        }

        return s_cameraStore->Get(cameraId);
    }

    const List<entity_id_t> &GetHoveredTexture()
    {
        return s_hoveredTextures;
    }

    void UnloadTexture(resource_id_t texture_id)
    {
        PROFILE_FUNCTION();

        if (s_textureStore->Get(texture_id) == nullptr)
        {
            NTT_ENGINE_WARN("The texture with the ID {} is already unloaded", texture_id);
            return;
        }

        try
        {
            s_graphicAPI->UnloadTexture(s_textureStore->Get(texture_id)->texture);
        }
        catch (const std::exception &e)
        {
            NTT_ENGINE_ERROR("Unloading the texture with the ID {} error: {}",
                             texture_id, e.what());
        }
        s_textureStore->Release(texture_id);
    }

    void RendererShutdown()
    {
        PROFILE_FUNCTION();
        ForEachFunc<resource_id_t, TextureInfo> func = [&](Ref<TextureInfo> texture, resource_id_t id)
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not unloaded, unloading ...", id);
            UnloadTexture(id);
        };

        s_textureStore->ForEach(func);

        ASSERT_M(s_textureStore->GetAvailableIds().size() == 0,
                 "The texture store is not empty");

        s_textureStore.reset();
        s_cameraStore.reset();
    }
} // namespace ntt