#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/editor/EditorWindow.hpp>

namespace ntt
{
    class ViewportWindow : public EditorWindow
    {
    public:
        ViewportWindow(f32 width, f32 height);
        ~ViewportWindow() override;

        void OnStartDraw();
        void OnStopDraw();

        void Init() override;
        void Update(ImGuiWindowFlags flags) override;
        void Shutdown() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
