#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/exception.hpp>

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <initializer_list>

namespace ntt
{
    using namespace log;
    using namespace exception;

    /**
     * List data structure
     */
    template <typename T>
    class List
    {
    public:
        List() { m_List = std::vector<T>(); }
        List(std::initializer_list<T> list) : m_List(list) {}
        List(const List<T> &list) : m_List(list.m_List) {}
        ~List() = default;

        /**
         * @param index: Can be positive or negative
         *      if positive: index from the start of the list
         *      if negative: index from the end of the list
         *      if the index is out of range, the
         *          out-of-range exception will be thrown
         */
        inline T Get(i64 index) const
        {
            auto stdIndex = ConvertNegativeIndexToPositive(index);
            if (stdIndex >= Length() || stdIndex < 0)
            {
                throw IndexOutOfRange();
            }

            return m_List[stdIndex];
        }

        inline T operator[](i64 index) const { return Get(index); }

        inline u32 Length() const { return m_List.size(); }

        inline void Add(T item) { m_List.push_back(item); }

        /**
         * @param index: Can be positive or negative
         *      if positive: index from the start of the list
         *      if negative: index from the end of the list
         *      if the index is out of range then nothing will be removed
         *          and a warning will be logged
         */
        inline void Remove(i64 index)
        {
            auto stdIndex = ConvertNegativeIndexToPositive(index);
            if (stdIndex >= Length() || stdIndex < 0)
            {
                NTT_ENGINE_WARN("The index is out of range, nothing will be removed");
                return;
            }

            m_List.erase(m_List.begin() + stdIndex);
        }

        /**
         * @param item: The item to be removed
         * @param callback: The comparison callback must be provided
         *      if the default comparison operator cannot be used with the type
         */
        inline void RemoveItem(T item, std::function<bool(const T &, const T &)> callback = [](const T &a, const T &b)
                                       { return a == b; })
        {
            m_List.erase(std::remove_if(m_List.begin(), m_List.end(), [item, callback](const T &listItem)
                                        { return callback(item, listItem); }),
                         m_List.end());
        }

        /**
         * If the start index and end index are out of range,
         *      the whole list will be returned
         *
         * @param start: The start index of the sublist, can be negative/positive
         * @param end: The end index of the sublist, can be negative/positive
         */
        inline List<T> SubList(i64 start, i64 end)
        {
            auto stdStart = ConvertNegativeIndexToPositive(start);
            auto stdEnd = ConvertNegativeIndexToPositive(end);

            if (start >= end)
            {
                return {};
            }

            auto begin = stdStart > 0 ? stdStart : 0;
            auto finish = stdEnd < Length() ? stdEnd : Length();

            List<T> subList;
            for (u32 i = begin; i < finish; i++)
            {
                subList.Add(m_List[i]);
            }
            return subList;
        }

        /**
         * Another version which does not return the object
         *      it will modify the reference of the object
         *      which is passed to the function
         */
        template <typename U>
        inline void Reduce(std::function<void(U &, const T &, const u32)> callback, U &reducedList)
        {
            for (u32 i = 0; i < m_List.size(); i++)
            {
                callback(reducedList, m_List[i], i);
            }
        }

        /**
         * @param list: The list to be compared with
         * @param callback: The comparison callback which
         *      must be provided if the default comparison
         *      operator cannot be used with the type
         */
        inline b8 Equals(const List<T> &list, std::function<bool(const T &, const T &)> callback = [](const T &a, const T &b)
                                              { return a == b; }) const
        {
            if (Length() != list.Length())
            {
                return FALSE;
            }

            for (u32 i = 0; i < Length(); i++)
            {
                if (callback(m_List[i], list.Get(i)) == FALSE)
                {
                    return FALSE;
                }
            }

            return TRUE;
        }

        /**
         * Use for iterating through the list
         */
        inline void ForEach(std::function<void(const T &, const u32)> callback)
        {
            for (u32 i = 0; i < Length(); i++)
            {
                callback(m_List[i], i);
            }
        }

        /**
         * Check if any item of the list satisfies the condition
         */
        inline b8 Any(std::function<b8(const T &, const u32)> callback)
        {
            for (u32 i = 0; i < Length(); i++)
            {
                if (callback(m_List[i], i))
                {
                    return true;
                }
            }
            return false;
        }

        /**
         * Check if all items of the list satisfy the condition
         */
        inline b8 All(std::function<b8(const T &, const u32)> callback)
        {
            for (u32 i = 0; i < Length(); i++)
            {
                if (!callback(m_List[i], i))
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
        inline List<U> Map(std::function<U(const T &, const u32)> callback)
        {
            List<U> newList;
            for (u32 i = 0; i < Length(); i++)
            {
                newList.Add(callback(m_List[i], i));
            }
            return newList;
        }

        /**
         * Convert to string with format [item1, item2, ...]
         */
        inline const char *ToString()
        {
            std::string buffer = "[";
            for (u32 i = 0; i < Length(); i++)
            {
                buffer += std::to_string(m_List[i]);
                if (i != Length() - 1)
                {
                    buffer += ", ";
                }
            }
            buffer += "]";
            return buffer.c_str();
        }

    protected:
        /**
         * Convert the negative index to positive index
         */
        i64 ConvertNegativeIndexToPositive(i64 index) const
        {
            if (index >= 0)
            {
                return index;
            }
            return m_List.size() + index;
        }

    private:
        std::vector<T> m_List;
    };
} // namespace ntt