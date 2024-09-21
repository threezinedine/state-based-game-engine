#pragma once
#include <NTTEngine/defines.hpp>
#include <vector>
#include <string>
#include <functional>
#include "list.hpp"

namespace ntt
{
    /**
     * the compare function must return:
     *  0 if the two items are equal
     *  >0 if the first item is greater than the second item
     *  <0 if the first item is less than the second item
     */

    template <typename T>
    class SortedList : public std::vector<T>
    {
    public:
        SortedList(
            std::function<i32(const T &, const T &)> comparator =
                [](const T &a, const T &b)
            { return a - b; })
            : std::vector<T>(),
              m_comparator(comparator)
        {
        }

        SortedList(const SortedList<T> &list)
            : std::vector<T>(list),
              m_comparator(list.m_comparator)
        {
        }

        void Add(T item)
        {
            u32 i = 0;

            if (this->size() == 0)
            {
                this->push_back(item);
                return;
            }

            for (i = 0; i < this->size(); i++)
            {
                if (m_comparator((*this)[i], item) >= 0)
                {
                    this->insert(this->begin() + i, item);
                    return;
                }
            }

            this->push_back(item);
        }

        void Remove(u32 index)
        {
            if (index >= this->size())
            {
                return;
            }
            this->erase(this->begin() + index);
        }

        void RemoveItem(T item)
        {
            auto it = std::lower_bound(this->begin(), this->end(), item);
            if (it != this->end() && *it == item)
            {
                this->erase(it);
            }
        }

        b8 Contains(T item) const
        {
            auto it = std::lower_bound(this->begin(), this->end(), item);
            return it != this->end() && *it == item;
        }

        b8 Equals(const SortedList<T> &list)
        {
            if (this->size() != list.size())
            {
                return FALSE;
            }

            for (u32 i = 0; i < this->size(); i++)
            {
                if (m_comparator((*this)[i], list[i]) != 0)
                {
                    return FALSE;
                }
            }

            return TRUE;
        }

        b8 operator==(const SortedList<T> &list) const
        {
            return Equals(list);
        }

        b8 operator==(const List<T> &list) const
        {
            if (this->size() != list.size())
            {
                return FALSE;
            }

            for (u32 i = 0; i < this->size(); i++)
            {
                if (m_comparator((*this)[i], list[i]) != 0)
                {
                    return FALSE;
                }
            }

            return TRUE;
        }

        const char *ToString() const
        {
            static std::string str = "[";
            str = "[";
            for (u32 i = 0; i < this->size(); i++)
            {
                str += std::to_string((*this)[i]);
                if (i < this->size() - 1)
                {
                    str += ", ";
                }
            }
            str += "]";
            return str.c_str();
        }

    private:
        std::function<i32(const T &, const T &)> m_comparator;
    };
} // namespace ntt
