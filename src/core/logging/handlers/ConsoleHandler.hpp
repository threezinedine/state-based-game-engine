#pragma once

#include "Handler.hpp"

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
        ConsoleHandler() = default;
        ~ConsoleHandler() override = default;

        void Handle(const LogMessage &message) override;
    };
} // namespace ntt::log
