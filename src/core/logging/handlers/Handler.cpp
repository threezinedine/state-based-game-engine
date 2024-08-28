#include "Handler.hpp"
#include "ConsoleHandler.hpp"
#include "TestingHandler.hpp"

namespace ntt::log
{
    void ExtractHandlers(u8 type, std::vector<Scope<Handler>> &handlers)
    {
        handlers.clear();

        // TODO: Create the mechanism to extract the handlers

        if (type == LOGGER_TESTING)
        {
            handlers.push_back(CreateScope<TestingHandler>());
        }
        else
        {
            handlers.push_back(CreateScope<ConsoleHandler>());
        }
    }
} // namespace ntt::log