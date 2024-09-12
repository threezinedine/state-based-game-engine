#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/structures/dictionary.hpp>

namespace ntt
{
    /**
     * An object contains whole information of a JSON which is
     *      used over the application. Only supports types:
     *      - u8, u16, u32, u64
     *      - i8, i16, i32, i64
     *      - f32, f64
     *      - b8, b32
     *      - String
     *      - JSON
     *      - List of all above types
     *      - Dictionary of all above types with key is String
     */
    class JSON
    {
    public:
        JSON();
        JSON(String data);
        JSON(const JSON &other);
        ~JSON();

        /**
         * Get Keys of the JSON object.
         */
        List<String> GetKeys() const;

        /**
         * Check whether the JSON object has the key or not.
         *
         * @param key The key to check in the JSON object.
         * @return True if the key is visible in the JSON object
         *      and has the type T
         */
        template <typename T>
        b8 Contains(const String &key) const;

        /**
         * Check whether the JSON object has the key
         *      which is a list of values or not.
         *
         * @param key The key to check in the JSON object.
         * @return True if the key is visible in the JSON object
         */
        template <typename T>
        b8 ContainsList(const String &key) const;

        /**
         * Get the value from the JSON object and if the key is not
         *      visible or the JSON is an array, then the default value
         *      will be returned.
         *
         * Some special cases like get the float values from the u32 value
         *      or other integer types, the value will be converted
         *      into the desired type, vice versa.
         *
         * @param key The key to get the value from the JSON object.
         * @param defaultValue The default value if the key is not visible
         */
        template <typename T>
        T Get(const String &key, T defaultValue = T()) const;

        /**
         * Assign value to the JSON object with the key.
         *
         * If the key is not existed, then the new key will be created.
         * If the key is existed, then the value will be replaced and
         *      a warning will be logged.
         */
        template <typename T>
        void Set(const String &key, T value);

        /**
         * Get all list of values from the JSON object and if the key is not
         *      visible or the JSON is not an array, then the default value
         *      Can get list of JSON objects.
         */
        template <typename T>
        List<T> GetList(const String &key) const;

        /**
         * Assign the list of values to the JSON object with the key.
         */
        template <typename T>
        void Set(const String &key, List<T> value);

        void operator=(const JSON &other);
        void operator=(String data);

        String ToString() const;

    private:
        class Impl;
        // Cannot detect the size of the Impl class
        //      so we need to use the pointer to the Impl class
        //      instead of the smart pointer
        Impl *m_Impl = nullptr;
    };
} // namespace ntt
