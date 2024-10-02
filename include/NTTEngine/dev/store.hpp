#pragma once
#include "dev.hpp"
#include <functional>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/sorted_list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/assertion.hpp>

namespace ntt::dev::store
{
    template <typename id_t, typename data_t>
    using ForEachFunc = std::function<void(Ref<data_t>, const id_t)>;

    template <typename data_t>
    using CompareFunc = std::function<b8(Ref<data_t>, Ref<data_t>)>;

    template <typename data_t, typename field_t>
    using GetFieldFunc = std::function<field_t(Ref<data_t>)>;

    /**
     * Provide a generic way to storing resources
     *      object in the engine.
     *
     * @tparam id_t The type of the id used to identify the object.
     * @tparam data_t The type of the object to store. The data inside
     *      this object must be able to be deleted automatically when the
     *      object is removed from the store (prefer using Scope<> rather than Ref<>).
     * @tparam unique_field_t The additional field to ensure the uniqueness of the object.
     *      (ex: a file should only be loaded once)
     */
    template <typename id_t, typename data_t>
    class Store
    {
    public:
        Store(
            id_t defaultId,
            id_t maxElement,
            CompareFunc<data_t> compareFunc = [](Ref<data_t> a, Ref<data_t> b)
            { return *a == *b; })
            : m_defaultId(defaultId), m_currentId(defaultId),
              m_max(maxElement), m_compareFunc(compareFunc),
              m_store({}), m_freedIds()
        {
            NTT_ENGINE_TRACE("The store is created");
        }

        ~Store()
        {
            m_store.clear();
            m_freedIds.clear();
            NTT_ENGINE_TRACE("The store is deleted");
        }

        /**
         * When new object, it will return the id of the object.
         *      which can be a completely new id or a reused id (if
         *      the unique_field is the same).
         */
        id_t Add(Ref<data_t> data)
        {
            if (m_store.size() >= m_max)
            {
                NTT_ENGINE_TRACE("The store is full, cannot add more elements");
                return m_defaultId;
            }

            if (Contains(data))
            {
                NTT_ENGINE_TRACE("The object is already stored in the store");

                auto availableIds = GetAvailableIds();

                for (auto id : availableIds)
                {
                    if (m_compareFunc(data, m_store[id]))
                    {
                        return id;
                    }
                }
                return m_defaultId;
            }

            if (m_freedIds.size() > 0)
            {
                auto id = m_freedIds[0];
                m_store[id] = data;
                m_freedIds.Remove(0);

                return id;
            }
            else
            {
                m_store.push_back(data);
                return m_currentId++;
            }
        }

        /**
         * Query the object which has the field value which
         *      matches the given value.
         */
        template <typename field_t>
        List<Ref<data_t>> GetByField(field_t value, GetFieldFunc<data_t, field_t> getField)
        {
            List<Ref<data_t>> result = {};
            auto availableIds = GetAvailableIds();

            for (auto id : availableIds)
            {
                if (getField(Get(id)) == value)
                {
                    result.push_back(Get(id));
                }
            }

            return result;
        }

        template <typename field_t>
        List<id_t> GetIdsByField(field_t value, GetFieldFunc<data_t, field_t> getField)
        {
            List<id_t> ids = {};
            auto availableIds = GetAvailableIds();

            for (auto i = 0; i < availableIds.size(); i++)
            {
                if (getField(Get(availableIds[i])) == value)
                {
                    ids.push_back(availableIds[i]);
                }
            }

            return ids;
        }

        /**
         * Query the object with the id.
         *      if the object is not found, it will return nullptr.
         */
        Ref<data_t> Get(id_t id)
        {
            if (!Contains(id))
            {
                NTT_ENGINE_TRACE("The id is not existed in the store");
                return nullptr;
            }

            return m_store[id];
        }

        /**
         * Check if the object is currently available in the store.
         */
        u8 Contains(id_t id)
        {
            if (id < m_defaultId || id >= m_currentId)
            {
                return FALSE;
            }

            if (m_store[id] == nullptr)
            {
                return FALSE;
            }

            return TRUE;
        }

        /**
         * Check if the unique field is already stored in the store.
         */
        u8 Contains(Ref<data_t> data)
        {
            for (auto i = 0; i < m_currentId; i++)
            {
                if (m_store[i] != nullptr && m_compareFunc(data, m_store[i]))
                {
                    return TRUE;
                }
            }

            return FALSE;
        }

        /**
         * Remove the object with the id. Then that id
         *      can be reused, if the id is not added,
         *      then nothing happens
         */
        void Release(id_t id)
        {
            auto data = Get(id);

            if (data != nullptr)
            {
                m_store[id] = nullptr;
                // data.reset();

                // ASSERT_M(data == nullptr, "The object is not deleted properly");

                // for (auto i = 0; i < m_freedIds.size(); i++)
                // {
                //     if (m_freedIds[i] > id)
                //     {
                //         m_freedIds.Add(i, id);
                //         return;
                //     }
                // }

                // m_freedIds.push_back(id);
                m_freedIds.Add(id);
            }
        }

        /**
         * Get all ids which their objects are currently stored.
         */
        List<id_t> GetAvailableIds()
        {
            List<id_t> availableIds = {};
            for (id_t i = 0; i < m_currentId; i++)
            {
                if (m_store[i] != nullptr)
                {
                    availableIds.push_back(i);
                }
            }
            return availableIds;
        }

        /**
         * Functions which will be called for each active object in the store.
         */
        void ForEach(ForEachFunc<id_t, data_t> func)
        {
            for (auto i = 0; i < m_currentId; i++)
            {
                if (m_store[i] != nullptr)
                {
                    func(m_store[i], i);
                }
            }
        }

    private:
        id_t m_defaultId;
        id_t m_currentId;
        id_t m_max;
        List<Ref<data_t>> m_store;
        CompareFunc<data_t> m_compareFunc;
        SortedList<id_t> m_freedIds;
    };
} // namespace ntt::dev::store
