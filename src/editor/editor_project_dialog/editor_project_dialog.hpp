#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/editor/types.hpp>

namespace ntt
{
    using namespace memory;

    struct EditorFileDialogData
    {
        // the string which is used for identifying the differenet file dialog
        //      around the editor
        String id;

        // the title of the file dialog
        String windowTitle;

        // callback which is used for checking any existance project in the selected
        //      folder
        std::function<b8(const String &)> checkExistance;

        // callback which is used for the ok button, the parameter is the override
        //      flag which is used for overriding the existing project.
        std::function<void(b8)> onOk;
    };

    class EditorFileDialog
    {
    public:
        EditorFileDialog(Ref<ProjectInfo> project,
                         Ref<EditorConfig> config,
                         const EditorFileDialogData &data);
        ~EditorFileDialog();

        void Open();

        void Update();

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
