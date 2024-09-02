#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/exception.hpp>
#include "list.hpp"
#include <map>
#include <initializer_list>
#include <utility>
#include <functional>

namespace ntt
{
    using namespace log;
    using namespace exception;

    template <typename K, typename V>
    class Dictionary
    {
    public:
        Dictionary() = default;
        Dictionary(std::initializer_list<std::pair<const K, V>> list) : m_Dict(list) {}

        inline V &Get(K key)
        {
            if (!Contains(key))
            {
                throw KeyNotFound();
            }

            return m_Dict[key];
        }
        inline V &operator[](K key) { return m_Dict[key]; }

        inline void Insert(K key, V value) { m_Dict[key] = value; }
        inline void Remove(K key) { m_Dict.erase(key); }

        inline bool Contains(K key) { return m_Dict.find(key) != m_Dict.end(); }
        inline u32 Count() { return m_Dict.size(); }
        inline List<V> Keys()
        {
            List<V> keys;
            for (auto &pair : m_Dict)
            {
                keys.PushBack(pair.first);
            }
            return keys;
        }

        inline List<V> Values()
        {
            List<V> values;
            for (auto &pair : m_Dict)
            {
                values.PushBack(pair.second);
            }
            return values;
        }

        /**
         * The callback will be called with each key-value pair in the dictionary
         */
        inline void ForEach(std::function<void(const K &, V &)> callback)
        {
            for (auto &pair : m_Dict)
            {
                callback(pair.first, pair.second);
            }
        }

        /**
         * The callback will be called with each key-value pair, and
         *      update the passed value reference with the new value
         */
        template <typename U>
        inline void Reduce(std::function<void(const K &, V &, U &)> callback, U &initialValue)
        {
            for (auto &pair : m_Dict)
            {
                callback(pair.first, pair.second, initialValue);
            }
        }

        inline void Clear() { m_Dict.clear(); }

        const char *ToString()
        {
            static std::string str = "{";
            for (auto &pair : m_Dict)
            {
                str += "\n\t" + pair.first + ": " + pair.second;
                if (pair != m_Dict.end() - 1)
                {
                    str += ",";
                }
            }
            str += "}";
            return str.c_str();
        }

    private:
        std::map<K, V> m_Dict;
    };
} // namespace ntt
