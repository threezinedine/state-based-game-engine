#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/structures/list.hpp>
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

    String String::SubString(u32 start) const
    {
        return String(m_Str.substr(start));
    }

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

    u32 String::FindFirst(String str, u32 startIndex) const
    {
        auto index = m_Str.find(str.RawString(), startIndex);
        if (index == std::string::npos)
        {
            return NotFound;
        }

        return index;
    }

    List<u32> String::FindAll(String str) const
    {
        List<u32> indexes;
        auto index = m_Str.find(str.RawString());
        while (index != std::string::npos)
        {
            indexes.push_back(index);
            index = m_Str.find(str.RawString(), index + 1);
        }

        return indexes;
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

    b8 String::Contains(const String &str) const
    {
        return m_Str.find(str.RawString()) != std::string::npos;
    }

    void String::Trim()
    {
        auto start = m_Str.find_first_not_of(" ");
        auto end = m_Str.find_last_not_of(" ");
        m_Str = m_Str.substr(start, end - start + 1);
    }

    List<String> String::Split(const String &delimiter) const
    {
        List<String> res;
        auto index = m_Str.find(delimiter.RawString());
        u32 lastIndex = 0;
        while (index != std::string::npos)
        {
            res.push_back(m_Str.substr(lastIndex, index - lastIndex));
            lastIndex = index + delimiter.Length();
            index = m_Str.find(delimiter.RawString(), lastIndex);
        }

        res.push_back(m_Str.substr(lastIndex, m_Str.length() - lastIndex));
        return res;
    }

    b8 String::MatchPattern(const String &pattern) const
    {
        u32 index = 0;
        b8 isMatch = TRUE;
        auto segment = pattern.Split("@");

        if (segment.size() == 0)
        {
            return *this == pattern;
        }

        for (u32 i = 0; i < segment.size(); i++)
        {
            auto str = segment[i];
            if (str.Length() == 0)
            {
                continue;
            }
            auto foundIndex = FindFirst(str, index);
            if (foundIndex == NotFound)
            {
                return FALSE;
            }
            index = foundIndex + str.Length();
        }

        return TRUE;
    }

    bool String::operator==(const String &str) const
    {
        return m_Str == str.m_Str;
    }

    bool String::operator!=(const String &str) const
    {
        return m_Str != str.m_Str;
    }

    bool String::operator<(const String &str) const
    {
        return m_Str < str.m_Str;
    }

    bool String::operator>(const String &str) const
    {
        return m_Str > str.m_Str;
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
