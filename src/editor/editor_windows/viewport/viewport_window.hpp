#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>
#include "../EditorWindow.hpp"

namespace ntt
{
    using namespace memory;

    class ViewportWindow : public EditorWindow
    {
    public:
        ViewportWindow(f32 width, f32 height);
        ~ViewportWindow() override;

        void OnStartDraw();
        void OnStopDraw();

        void Init() override;
        void Update() override;
        void Shutdown() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
