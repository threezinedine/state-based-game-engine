#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/logging/logging.hpp>
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
    class Dictionary : public std::map<K, V>
    {
    public:
        Dictionary() : std::map<K, V>() {}
        Dictionary(std::initializer_list<std::pair<const K, V>> list) : std::map<K, V>(list) {}
        Dictionary(const Dictionary<K, V> &dict) : std::map<K, V>(dict) {}

        inline bool Contains(K key) { return this->find(key) != this->end(); }

        List<K> Keys()
        {
            List<K> keys;
            for (auto &pair : (*this))
            {
                keys.push_back(pair.first);
            }
            return keys;
        }

        List<V> Values()
        {
            List<V> values;
            for (auto &pair : (*this))
            {
                values.push_back(pair.second);
            }
            return values;
        }

        /**
         * The callback will be called with each key-value pair in the dictionary
         */
        void ForEach(std::function<void(const K &, V &)> callback)
        {
            for (auto &pair : (*this))
            {
                callback(pair.first, pair.second);
            }
        }

        /**
         * The callback will be called with each key-value pair, and
         *      update the passed value reference with the new value
         */
        template <typename U>
        void Reduce(std::function<void(const K &, V &, U &)> callback, U &initialValue)
        {
            for (auto &pair : (*this))
            {
                callback(pair.first, pair.second, initialValue);
            }
        }

        const char *ToString()
        {
            static std::string str = "{";
            for (auto &pair : (*this))
            {
                str += "\n\t" + pair.first + ": " + pair.second;
                if (pair != this->end() - 1)
                {
                    str += ",";
                }
            }
            str += "}";
            return str.c_str();
        }
    };
} // namespace ntt
