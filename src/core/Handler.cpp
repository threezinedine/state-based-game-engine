#include "Handler.hpp"
#include "ConsoleHandler.hpp"

namespace ntt::log
{
    void ExtractHandlers(u8 type, std::vector<Scope<Handler>> &handlers)
    {
        handlers.clear();

        // TODO: Create the mechanism to extract the handlers

        handlers.push_back(CreateScope<ConsoleHandler>());
    }
} // namespace ntt::log
