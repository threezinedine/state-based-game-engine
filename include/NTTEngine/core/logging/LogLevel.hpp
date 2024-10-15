#pragma once

namespace ntt
{
    /**
     * The LogLevel represents the importance of the message
     *      in log system.
     */
    enum LogLevel
    {
        FATAL = 0, ///< The error that crashes the whole application
        ERROR = 1, ///< The error that can be handled but the application
                   ///<     cannot continue to run (like network error, etc.)
        WARN = 2,  ///< The exceptions which can be handled easily or ignored (
                   ///<     like missing resources, etc.)
        INFO = 3,  ///< The normal information such as intialized the window, etc.
        DEBUG = 4, ///< The detail information which can be valuable for debugging
                   ///<     such as the position of the object, size of the image, etc.
        TRACE = 5  ///< The information which is representing the flow of the application
                   ///<     such as the function is called, etc. (should use in profiling)
    };

} // namespace ntt
