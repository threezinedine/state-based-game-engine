#pragma once
#include "LogMessage.hpp"
#include <NTTEngine/core/object.hpp>

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

    class Handler : public Object
    {
    public:
        Handler() {}
        virtual ~Handler() {}

        /**
         * Receive the message from the logger and does something for
         *      printing, storing, these information with the certain
         *      format
         */
        virtual void Handle(const LogMessage &message) = 0;
    };
} // namespace ntt::log
