#pragma once
#include "../LogMessage.hpp"

// TODO: Temporary includes
#include <vector>
// End temporary includes

namespace ntt::log
{
    /**
     * The instance of this interface which will receive the message from the logger
     *      (if it should be handled) and handles it
     *
     * Other handler should be implemented from this interface to handle the message
     */
    class Handler
    {
    public:
        virtual ~Handler() = default;

        /**
         * Receive the message from the logger and does something for
         *      printing, storing, these information with the certain
         *      format
         */
        virtual void Handle(const LogMessage &message) = 0;
    };

    /**
     * Needed handlers extracted from the
     *      type of the logger
     *
     * @param type: The type of the logger like `LOGGER_CONSOLE`,
     *      `LOGGER_FILE`, `LOGGER_CONSOLE | LOGGER_FILE`, etc.
     *
     * @param handlers: The list of handlers which will be extracted
     *      from the type, the reference of the list will be changed
     *      to the list of handlers, if the `handlers` contains any
     *      handler, it will be cleared before extracting
     */
    void ExtractHandlers(u8 type, std::vector<Scope<Handler>> &handlers);
} // namespace ntt::log
