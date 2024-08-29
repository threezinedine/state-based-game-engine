#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/logging.hpp>
#include <cstdarg>

namespace ntt
{
    using namespace log;

    String::String() : m_Str("") {}

    String::String(const char *str, ...)
    {
        char buffer[1024];
        va_list args;
        va_start(args, str);
        vsnprintf(buffer, sizeof(buffer), str, args);
        va_end(args);

        m_Str = std::string(buffer);
    }

    String::String(const std::string &str) : m_Str(str) {}
    String::String(const String &str) : m_Str(str.m_Str) {}

    String::~String() {}

    String String::SubString(u32 start, u32 end) const
    {
        if (end > m_Str.length() - 1)
        {
            NTT_ENGINE_WARN("End index is greater than the length of the string"
                            ", returning the whole string");
            return String(m_Str);
        }

        return String(m_Str.substr(start, end));
    }

    u32 String::FindFirst(String str) const
    {
        auto index = m_Str.find(str.RawString());
        if (index == std::string::npos)
        {
            return NotFound;
        }

        return index;
    }

    void String::Replace(const String &oldStr, const String &newStr)
    {
        while (1)
        {
            auto index = m_Str.find(oldStr.RawString());
            if (index != std::string::npos)
            {
                m_Str.replace(index, oldStr.Length(), newStr.RawString());
            }
            else
            {
                break;
            }
        }
    }

    void String::Concat(const String &str, const String &format)
    {
        if (format.Length() == 0)
        {
            m_Str += str.m_Str;
        }
        else
        {
            auto copy = format;
            copy.Replace("{src}", m_Str);
            copy.Replace("{str}", str);
            m_Str = copy.m_Str;
        }
    }

    bool String::operator==(const String &str)
    {
        return m_Str == str.m_Str;
    }

    void String::operator=(const String &str)
    {
        m_Str = str.m_Str;
    }

    String String::operator+(const String &str)
    {
        auto res = String(m_Str);
        res.Concat(str);
        return res;
    }

    String String::operator+(const char *str)
    {
        auto res = String(m_Str);
        res.Concat(str);
        return res;
    }

    String String::operator+(const std::string &str)
    {
        m_Str += std::string(str);
        return String(m_Str);
    }

    void String::operator+=(const String &str)
    {
        m_Str += str.m_Str;
    }

    void String::operator+=(const char *str)
    {
        m_Str += std::string(str);
    }

    void String::operator+=(const std::string &str)
    {
        m_Str += str;
    }

    std::string String::RawString() const
    {
        return m_Str;
    }
} // namespace ntt
