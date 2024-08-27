#include <NTTEngine/core/String.hpp>

namespace ntt
{
    String::String(const char *str)
        : m_Str(str)
    {
    }

    String::String()
        : m_Str("")
    {
    }

    String::~String()
    {
    }
} // namespace ntt
