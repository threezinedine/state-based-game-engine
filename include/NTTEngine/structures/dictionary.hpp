#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/exception.hpp>
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

        inline bool Contains(K key) { return m_Dict.find(key) != m_Dict.end(); }

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

    private:
        std::map<K, V> m_Dict;
    };
} // namespace ntt
