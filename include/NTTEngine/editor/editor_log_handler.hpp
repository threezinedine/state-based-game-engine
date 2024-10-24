#pragma once
#include <NTTEngine/core/logging/Handler.hpp>

namespace ntt
{
    class EditorLogHandler : public Handler
    {
    public:
        EditorLogHandler();
        ~EditorLogHandler();

        void Handle(const LogMessage &message) override;
    };
} // namespace ntt
