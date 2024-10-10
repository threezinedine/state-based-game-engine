#pragma once
#include <NTTEngine/defines.hpp>
#include "EditorWindow.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    class OpenClosableWindow : public EditorWindow
    {
    public:
        OpenClosableWindow();
        ~OpenClosableWindow();

        virtual void DrawMenuItem();
        virtual void Open();
        virtual void Close();

        void Init() override;
        void Update() override;
        void Shutdown() override;

    protected:
        virtual void InitImpl() = 0;
        virtual void UpdateImpl(b8 *p_open) = 0;
        virtual void ShutdownImpl() = 0;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
}