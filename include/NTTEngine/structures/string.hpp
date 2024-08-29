#pragma once
#include <NTTEngine/defines.hpp>
#include <string>
#include "list.hpp"

namespace ntt
{
    class String
    {
    public:
        String();
        String(const char *str, ...);
        String(const std::string &str);
        String(const String &str);
        ~String();

        /**
         * @param start: The start index of the substring
         * @param end: The end index of the substring, if
         *      the end index is greater than the length of the string
         *      then return the whole string and warning will be logged
         */
        String SubString(u32 start, u32 end) const;

        inline u32 Length() const { return m_Str.length(); }

        /**
         * @param str: The string to find the first occurrence index
         *      in the current string, if the string is not found
         *      then return String::NotFound
         * @param index: The index to start finding the string
         */
        u32 FindFirst(String str, u32 startIndex = 0) const;

        /**
         * @param str: The string to find all the occurrence indexes
         */
        List<u32> FindAll(String str) const;

        /**
         * Remove all spaces at the beginning and the end of the string
         */
        void Trim();

        /**
         * Replace all occurrences of the old string with the new string
         * @param oldStr: The string to be replaced
         * @param newStr: The string to replace the old string
         */
        void Replace(const String &oldStr, const String &newStr);

        /**
         * Concat the current string with other string
         *
         * @param str: The string to be concatenated with the current string
         * @param format: Optional, if the format is empty string
         *      the the str will be concatenated at the end of the current string
         *      else the str will be concatenated follow by the sign {str},
         *      the current value will replace with {src} in the format string.
         *      - The number of {src} and {str} are not limited
         *      - Ex: format "Hello, {src} + {str}" with current string "World"
         *          and passed string "Universe" will return "Hello, World + Universe"
         *      - Ex: format "Hello, {str}" with current string "World"
         *          and passed string "Universe" will return "Hello, Universe"
         */
        void Concat(const String &str, const String &format = "");

        /**
         * Split the string by the delimiter
         *
         * @param delimiter: The delimiter to split the string,
         *      if the delimiter is not found then return
         *      the list with only the current string
         */
        List<String> Split(const String &delimiter) const;

        /**
         * Pattern matching with the current string
         *      where as all `{}` will be ignored
         *      when checking whether the pattern is matched
         *
         * @param pattern: The pattern to be matched with the current string
         *      - Ex: current string "Hello, World first" with pattern "Hello, {} first"
         *          will return true
         *      - Ex: current string "Hello, World first" with pattern "Hello, {} second"
         *          will return false
         */
        b8 MatchPattern(const String &pattern) const;

        /**
         * If 2 strings has the same length and the same characters
         *     then they are equal
         */
        bool operator==(const String &str) const;
        bool operator!=(const String &str) const;
        bool operator<(const String &str) const;
        bool operator>(const String &str) const;

        /**
         * Assign the value of the string to the current string
         */
        void operator=(const String &str);

        /**
         * Concat the current string with other string
         */
        String operator+(const String &str);
        String operator+(const char *str);
        String operator+(const std::string &str);

        void operator+=(const String &str);
        void operator+=(const char *str);
        void operator+=(const std::string &str);

        /**
         * Hash the string to a 32-bit integer
         */
        inline std::size_t Hash() const
        {
            return std::hash<std::string>{}(m_Str);
        }

        std::string RawString() const;

        // Represents the index of something that does
        //      not exist in the string
        static const u32 NotFound = -1;

    private:
        std::string m_Str;
    };

} // namespace ntt

template <>
struct std::hash<ntt::String>
{
    std::size_t operator()(const ntt::String &str) const
    {
        return str.Hash();
    }
};