#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/exception.hpp>
#include <NTTEngine/core/formatter.hpp>

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <initializer_list>

namespace ntt
{
    using namespace exception;

    template <typename T>
    using ComparisionCallback = std::function<b8(const T &, const T &)>;

    /**
     * List data structure
     */
    template <typename T>
    class List : public std::vector<T>
    {
    public:
        List() : std::vector<T>() {}
        List(std::initializer_list<T> list) : std::vector<T>(list) {}
        List(const List<T> &list) : std::vector<T>(list) {}

        /**
         * Add the item at the specific index
         *
         * @param index: Can be positive only
         *      if the index is out of range, the item will be added to the end
         *
         * @param item: The item to be added
         */
        void Add(u32 index, T item)
        {
            if (index >= this->size())
            {
                this->push_back(item);
                return;
            }
            this->insert(this->begin() + index, item);
        }

        b8 Contains(T item, ComparisionCallback<T> callback = [](const T &a, const T &b)
                            { return a == b; }) const
        {
            return std::find(this->begin(), this->end(), item) != this->end();
        }

        /**
         * @param index: Can be positive or negative
         *      if positive: index from the start of the list
         *      if the index is out of range then nothing will be removed
         *          and a warning will be logged
         */
        void Remove(u32 index)
        {
            if (index >= this->size() || index < 0)
            {
                return;
            }

            this->erase(this->begin() + index);
        }

        /**
         * @param item: The item to be removed
         * @param callback: The comparison callback must be provided
         *      if the default comparison operator cannot be used with the type
         */
        void RemoveItem(T item, ComparisionCallback<T> callback = [](const T &a, const T &b)
                                { return a == b; })
        {
            this->erase(std::remove_if(
                            this->begin(),
                            this->end(),
                            [item, callback](const T &listItem)
                            { return callback(item, listItem); }),
                        this->end());
        }

        /**
         * If the start index and end index are out of range,
         *      the whole list will be returned
         *
         * @param start: The start index of the sublist
         * @param end: The end index of the sublist
         */
        List<T> SubList(u32 start, u32 end)
        {
            if (start >= end)
            {
                return {};
            }

            auto begin = start > 0 ? start : 0;
            auto finish = end < this->size() ? end : this->size();

            List<T> subList;
            for (u32 i = begin; i < finish; i++)
            {
                subList.push_back((*this)[i]);
            }
            return subList;
        }

        /**
         * Concatenate another list to the current list
         *
         * @param list: The list to be extended
         */
        void Extend(const List<T> &list)
        {
            for (u32 i = 0; i < list.size(); i++)
            {
                this->push_back(list[i]);
            }
        }

        /**
         * @param list: The list to be compared with
         * @param callback: The comparison callback which
         *      must be provided if the default comparison
         *      operator cannot be used with the type
         */
        b8 Equals(
            const List<T> &list,
            ComparisionCallback<T> callback = [](const T &a, const T &b)
            { return a == b; }) const
        {
            if (this->size() != list.size())
            {
                return FALSE;
            }

            for (u32 i = 0; i < this->size(); i++)
            {
                if (callback((*this)[i], list[i]) == FALSE)
                {
                    return FALSE;
                }
            }

            return TRUE;
        }

        /**
         * Sorting the list with the provided callback for comparison
         */
        void Sorted(std::function<b8(const T &, const T &)> callback =
                        [](const T &a, const T &b)
                    { return a < b; })
        {
            std::sort(this->begin(), this->end(), callback);
        }

        b8 operator==(const List<T> &list) const { return Equals(list); }

        /**
         * Use for iterating through the list
         */
        void ForEach(std::function<void(const T &, const u32)> callback)
        {
            for (u32 i = 0; i < this->size(); i++)
            {
                callback((*this)[i], i);
            }
        }

        /**
         * Check if any item of the list satisfies the condition
         */
        b8 Any(std::function<b8(const T &, const u32)> callback)
        {
            for (u32 i = 0; i < this->size(); i++)
            {
                if (callback((*this)[i], i))
                {
                    return true;
                }
            }
            return false;
        }

        /**
         * Check if all items of the list satisfy the condition
         */
        b8 All(std::function<b8(const T &, const u32)> callback)
        {
            for (u32 i = 0; i < this->size(); i++)
            {
                if (!callback((*this)[i], i))
                {
                    return false;
                }
            }
            return true;
        }

        /**
         * Create a new list with the same number of items
         *      which each item can be created from
         *      the current list item via the callback
         */
        template <typename U>
        List<U> Map(std::function<U(const T &, const u32)> callback)
        {
            List<U> newList;
            for (u32 i = 0; i < this->size(); i++)
            {
                newList.push_back(callback((*this)[i], i));
            }
            return newList;
        }

        /**
         * Copy all items of the list to a new list
         *      but does not deep copy (the pointer elements
         *      still point to the same memory location)
         */
        List<T> Copy()
        {
            List<T> newList;
            for (u32 i = 0; i < this->size(); i++)
            {
                newList.push_back((*this)[i]);
            }
            return newList;
        }

        /**
         * Convert to string with format [item1, item2, ...]
         */
        const char *ToString()
        {
            static std::string buffer = "[";

            buffer = "[";
            for (u32 i = 0; i < this->size(); i++)
            {
                buffer += std::to_string((*this)[i]);
                if (i != this->size() - 1)
                {
                    buffer += ", ";
                }
            }
            buffer += "]";
            return buffer.c_str();
        }
    };
} // namespace ntt