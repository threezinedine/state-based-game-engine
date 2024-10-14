#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/editor/OpenClosableWindow.hpp>

namespace ntt
{
    class NewProjectWindow : public OpenClosableWindow
    {
    public:
        NewProjectWindow(Ref<ProjectInfo> project, Ref<EditorConfig> config);
        ~NewProjectWindow() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
