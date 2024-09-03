#include "Handler.hpp"
#include "ConsoleHandler.hpp"
#include "TestingHandler.hpp"

namespace ntt::log
{
    void ExtractHandlers(LoggerType type, std::vector<Scope<Handler>> &handlers)
    {
        handlers.clear();

        if (type & LOGGER_TESTING)
        {
            handlers.push_back(CREATE_SCOPE(TestingHandler));
        }

        if (type & LOGGER_CONSOLE)
        {
            handlers.push_back(CREATE_SCOPE(ConsoleHandler));
        }
    }
} // namespace ntt::log
