#include <nlohmann/json.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <string>

namespace ntt
{
    class JSON::Impl
    {
    public:
        nlohmann::json data;
    };

    JSON::JSON()
        : m_Impl(new Impl())
    {
    }

    JSON::JSON(String data)
        : JSON()
    {
        m_Impl->data = nlohmann::json::parse(data.RawString());
    }

    JSON::JSON(const JSON &other)
        : JSON()
    {
        m_Impl->data = other.m_Impl->data;
    }

    JSON::~JSON()
    {
        if (m_Impl != nullptr)
        {
            delete m_Impl;
            m_Impl = nullptr;
        }
    }

    void JSON::operator=(const JSON &other)
    {
        m_Impl->data = other.m_Impl->data;
    }

    void JSON::operator=(String data)
    {
        m_Impl->data = nlohmann::json::parse(data.RawString());
    }

#define GENERAL_CHECK_TYPE(typeName)  \
    template <typename T>             \
    struct typeName : std::false_type \
    {                                 \
    };

#define TYPE_CORRECT(type, typeName)       \
    template <>                            \
    struct typeName<type> : std::true_type \
    {                                      \
    };

    GENERAL_CHECK_TYPE(IsUnsignedInteger)
    TYPE_CORRECT(u8, IsUnsignedInteger);
    TYPE_CORRECT(u16, IsUnsignedInteger);
    TYPE_CORRECT(u32, IsUnsignedInteger);
    TYPE_CORRECT(u64, IsUnsignedInteger);
    TYPE_CORRECT(i8, IsUnsignedInteger);
    TYPE_CORRECT(i16, IsUnsignedInteger);
    TYPE_CORRECT(i32, IsUnsignedInteger);
    TYPE_CORRECT(i64, IsUnsignedInteger);
    TYPE_CORRECT(f32, IsUnsignedInteger);
    TYPE_CORRECT(f64, IsUnsignedInteger);

    GENERAL_CHECK_TYPE(IsSignedInteger)
    TYPE_CORRECT(u8, IsSignedInteger);
    TYPE_CORRECT(u16, IsSignedInteger);
    TYPE_CORRECT(u32, IsSignedInteger);
    TYPE_CORRECT(u64, IsSignedInteger);
    TYPE_CORRECT(i8, IsSignedInteger);
    TYPE_CORRECT(i16, IsSignedInteger);
    TYPE_CORRECT(i32, IsSignedInteger);
    TYPE_CORRECT(i64, IsSignedInteger);
    TYPE_CORRECT(f32, IsSignedInteger);
    TYPE_CORRECT(f64, IsSignedInteger);

    GENERAL_CHECK_TYPE(IsFloat)
    TYPE_CORRECT(u8, IsFloat);
    TYPE_CORRECT(u16, IsFloat);
    TYPE_CORRECT(u32, IsFloat);
    TYPE_CORRECT(u64, IsFloat);
    TYPE_CORRECT(i8, IsFloat);
    TYPE_CORRECT(i16, IsFloat);
    TYPE_CORRECT(i32, IsFloat);
    TYPE_CORRECT(i64, IsFloat);
    TYPE_CORRECT(f32, IsFloat);
    TYPE_CORRECT(f64, IsFloat);

    GENERAL_CHECK_TYPE(IsBoolean)
    TYPE_CORRECT(b8, IsBoolean);
    TYPE_CORRECT(b32, IsBoolean);

    GENERAL_CHECK_TYPE(IsString)
    TYPE_CORRECT(String, IsString);

    template <typename T>
    struct IsJSON : std::false_type
    {
    };

    template <>
    struct IsJSON<JSON> : std::true_type
    {
    };

    template <typename T>
    b8 IsType(const nlohmann::json &data)
    {
        b8 result = false;
        if constexpr (IsUnsignedInteger<T>::value)
        {
            result |= data.is_number_unsigned();
        }
        if constexpr (IsSignedInteger<T>::value)
        {
            result |= data.is_number_integer();
        }
        if constexpr (IsFloat<T>::value)
        {
            result |= data.is_number_float();
        }
        if constexpr (IsBoolean<T>::value)
        {
            result |= data.is_boolean();
        }
        if constexpr (IsString<T>::value)
        {
            result |= data.is_string();
        }
        if constexpr (IsJSON<T>::value)
        {
            result |= data.is_object();
        }

        return result;
    }

    template <typename T>
    b8 JSON::Contains(const String &key) const
    {
        auto keyStr = key.RawString();
        return m_Impl->data.contains(keyStr) && IsType<T>(m_Impl->data[keyStr]);
    }

    template <typename T>
    b8 JSON::ContainsList(const String &key) const
    {
        auto keyStr = key.RawString();
        if (!m_Impl->data.contains(keyStr))
        {
            return FALSE;
        }
        auto obj = m_Impl->data[keyStr];

        b8 result = TRUE;

        if (obj.is_array())
        {
            for (auto &item : obj)
            {
                result &= IsType<T>(item);
            }
        }
        else
        {
            return FALSE;
        }

        return result;
    }

    String GetStringFrom(const nlohmann::json &data);

    template <typename T>
    T JSON::Get(const String &key, T defaultValue) const
    {
        auto keyStr = key.RawString();
        if (m_Impl->data.contains(keyStr) && IsType<T>(m_Impl->data[keyStr]))
        {
            if constexpr (IsString<T>::value)
            {
                return GetStringFrom(m_Impl->data[keyStr]);
            }
            else if constexpr (IsJSON<T>::value)
            {
                return JSON(m_Impl->data[keyStr].dump());
            }
            else
            {
                return m_Impl->data[keyStr].get<T>();
            }
        }

        return defaultValue;
    }

    template <typename T>
    List<T> JSON::GetList(const String &key) const
    {
        auto keyStr = key.RawString();
        List<T> result;
        if (m_Impl->data.contains(keyStr) && m_Impl->data[keyStr].is_array())
        {
            for (auto &item : m_Impl->data[keyStr])
            {
                if constexpr (IsString<T>::value)
                {
                    result.Add(GetStringFrom(item));
                }
                else if constexpr (IsJSON<T>::value)
                {
                    result.Add(JSON(item.dump()));
                }
                else
                {
                    result.Add(item.get<T>());
                }
            }

            return result;
        }
        return result;
    }

    String GetStringFrom(const nlohmann::json &data)
    {
        return String(data.get<std::string>());
    }

#define INSTANTIATE_GET_METHODS(type)                                          \
    template b8 JSON::Contains<type>(const String &key) const;                 \
    template b8 JSON::ContainsList<type>(const String &key) const;             \
    template type JSON::Get<type>(const String &key, type defaultValue) const; \
    template List<type> JSON::GetList<type>(const String &key) const;

    INSTANTIATE_GET_METHODS(u8);
    INSTANTIATE_GET_METHODS(u16);
    INSTANTIATE_GET_METHODS(u32);
    INSTANTIATE_GET_METHODS(u64);

    INSTANTIATE_GET_METHODS(i8);
    INSTANTIATE_GET_METHODS(i16);
    INSTANTIATE_GET_METHODS(i32);
    INSTANTIATE_GET_METHODS(i64);

    INSTANTIATE_GET_METHODS(f32);
    INSTANTIATE_GET_METHODS(f64);

    INSTANTIATE_GET_METHODS(b8);

    INSTANTIATE_GET_METHODS(String);
    INSTANTIATE_GET_METHODS(JSON);

    // template b8 JSON::Contains<JSON>(const String &key) const;
    // template JSON JSON::Get<JSON>(const String &key, JSON defaultValue) const;
} // namespace ntt
