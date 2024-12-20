#pragma once
#include <NTTEngine/defines.hpp>
#include <exception>
#include <stdexcept>

namespace ntt
{
    class Exception : public std::exception
    {
    public:
        Exception(const char *message) : m_Message(message) {}

        inline const char *what() const noexcept override { return m_Message; }

    private:
        const char *m_Message;
    };

    class IndexOutOfRange : public Exception
    {
    public:
        IndexOutOfRange() : Exception("Index out of range") {}
    };

    class KeyNotFound : public Exception
    {
    public:
        KeyNotFound() : Exception("Key not found in the dictionary") {}
    };
} // namespace ntt
