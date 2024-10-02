#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/structures/list.hpp>
#include <any>

namespace ntt
{
    String format(const String &str);

#define CHECK_AND_CAST(checkType)                                 \
    template <>                                                   \
    inline String format(const String &str, const checkType &arg) \
    {                                                             \
        String newStr = str;                                      \
        newStr.Replace("{}", String(std::to_string(arg)), FALSE); \
        return newStr;                                            \
    }

    template <typename T>
    inline String format(const String &str, const T &arg)
    {
        auto newStr = str;
        newStr.Replace("{}", "\"Not supported type\"");
        return newStr;
    }

    CHECK_AND_CAST(u16)
    CHECK_AND_CAST(u32)
    CHECK_AND_CAST(u64)
    CHECK_AND_CAST(i8)
    CHECK_AND_CAST(i16)
    CHECK_AND_CAST(i32)
    CHECK_AND_CAST(i64)
    CHECK_AND_CAST(f32)
    CHECK_AND_CAST(f64)
    CHECK_AND_CAST(bool)

    inline String format(const String &str, const char *arg)
    {
        String newStr = str;
        newStr.Replace("{}", String(arg), FALSE);
        return newStr;
    }

    template <>
    inline String format(const String &str, const std::string &arg)
    {
        String newStr = str;
        newStr.Replace("{}", String(arg), FALSE);
        return newStr;
    }

    template <>
    inline String format(const String &str, const String &arg)
    {
        String newStr = str;
        newStr.Replace("{}", arg, FALSE);
        return newStr;
    }

    template <typename T, typename... Args>
    inline String format(const String &str, const T &arg, Args... args)
    {
        auto newStr = format(str, arg);
        return format(newStr, args...);
    }
} // namespace ntt
