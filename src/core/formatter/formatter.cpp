#include <NTTEngine/core/formatter.hpp>

namespace ntt
{
    // #define CHECK_AND_CAST_IMPL(checkType)                            \
//     template <>                                                   \
//     String format(const String str, checkType arg)                \
//     {                                                             \
//         String newStr = str;                                      \
//         newStr.Replace("{}", String(std::to_string(arg)), FALSE); \
//         return newStr;                                            \
//     }

    //     CHECK_AND_CAST_IMPL(u16)
    //     CHECK_AND_CAST_IMPL(u32)
    //     CHECK_AND_CAST_IMPL(u64)
    //     CHECK_AND_CAST_IMPL(i8)
    //     CHECK_AND_CAST_IMPL(i16)
    //     CHECK_AND_CAST_IMPL(i32)
    //     CHECK_AND_CAST_IMPL(i64)
    //     CHECK_AND_CAST_IMPL(f32)
    //     CHECK_AND_CAST_IMPL(f64)
    //     CHECK_AND_CAST_IMPL(bool)

    //     String format(const String str, const char *arg)
    //     {
    //         String newStr = str;
    //         newStr.Replace("{}", String(arg), FALSE);
    //         return newStr;
    //     }

    //     String format(const String str, const std::string arg)
    //     {
    //         String newStr = str;
    //         newStr.Replace("{}", String(arg), FALSE);
    //         return newStr;
    //     }

    //     String format(const String str, String arg)
    //     {
    //         String newStr = str;
    //         newStr.Replace("{}", String(std::any_cast<const char *>(arg)), FALSE);
    //         return newStr;
    //     }

    String format(const String &str)
    {
        return str;
    }
} // namespace ntt
