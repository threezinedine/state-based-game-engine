#pragma once

#include <NTTEngine/core/logging/Handler.hpp>

namespace ntt::log
{
    /**
     * Print out all the log messages to the terminal
     *
     * Currently does not support coloring the output (//TODO add later)
     */
    class ConsoleHandler : public Handler
    {
    public:
        ConsoleHandler();
        ~ConsoleHandler() override {}

        void Handle(const LogMessage &message) override;
    };
} // namespace ntt::log
