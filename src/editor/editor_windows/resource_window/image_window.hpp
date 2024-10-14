#pragma once
#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    using namespace memory;

    class ImageWindow : public OpenClosableWindow
    {
    public:
        ImageWindow(const String &file, u8 numRow = 1, u8 numCol = 1);
        ~ImageWindow() override;

        void ChangeImage(const String &file, u8 numRow = 1, u8 numCol = 1);
        void ChangeGrid(u8 numRow, u8 numCol);

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
