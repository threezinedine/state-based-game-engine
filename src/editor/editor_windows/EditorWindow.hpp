#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    class EditorWindow
    {
    public:
        ~EditorWindow() = default;

        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Shutdown() = 0;
    };

} // namespace ntt
