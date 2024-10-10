#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/memory.hpp>
#include "../types.hpp"

namespace ntt
{
    using namespace memory;

    struct EditorFileDialogData
    {
        String id;
        String windowTitle;
        std::function<b8(const String &)> checkExistance;
        std::function<void()> onOk;
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
