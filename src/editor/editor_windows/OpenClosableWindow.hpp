#pragma once
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/defines.hpp>
#include "EditorWindow.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    /**
     * The base class for the window which can be visible or invisible during
     *      the runtime of the editor. User can interact with these windows via
     *      View menu or itself to make visible or hide them.
     */
    class OpenClosableWindow : public EditorWindow
    {
    public:
        OpenClosableWindow();
        OpenClosableWindow(const String &name);
        virtual ~OpenClosableWindow() override;

        /**
         * Used under the View menu to draw the menu item for controlling the
         *      window's visibility
         */
        virtual void DrawMenuItem();

        /**
         * Another way to make the window visible from the outside (not just the
         *      View menu)
         */
        virtual void Open();

        /**
         * Another way to make the window invisible from the outside (not just the
         *      View menu)
         */
        virtual void Close();

        // ========================================================================
        // Not override the methods below
        // ========================================================================

        void Init() override;
        void Update(ImGuiWindowFlags flags) override;
        void Shutdown() override;

        // ========================================================================
        // Not override the methods above
        // ========================================================================

    protected:
        /**
         * Implemnt this interface for configuring the starting window
         */
        virtual void InitImpl() = 0;

        /**
         * Implemnt this interface for updating the window
         *
         * @param p_open: The pointer to the boolean value which represent the
         *      visibility of the window (TRUE: visible, FALSE: invisible)
         */
        virtual void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) = 0;

        /**
         * Implemnt this interface for releasing all the resources of the window
         */
        virtual void ShutdownImpl() = 0;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
}