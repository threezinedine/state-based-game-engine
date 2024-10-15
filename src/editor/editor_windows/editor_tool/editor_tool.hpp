#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    enum ToolType
    {
        MOVE,
        SCALE,
        ROTATE,
    };

    class EditorTool : public OpenClosableWindow
    {
    public:
        EditorTool();
        ~EditorTool() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    void EditorToolInit();

    /**
     * Drawing the tool window with the ToolType options, if the tool is selected
     *      then the NTT_EDITOR_TOOL_TYPE_CHANGED event will be triggered
     */
    void EditorToolUpdate();

    void EditorToolShutdown();
} // namespace ntt
