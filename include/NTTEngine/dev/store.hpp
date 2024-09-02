#pragma once
#include "dev.hpp"
#include <functional>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>

namespace ntt::dev::store
{
    template <typename data_t, typename unique_field_t>
    using GetUniqueFieldFunc = std::function<unique_field_t(const data_t &)>;

    template <typename id_t, typename data_t>
    using ForEachFunc = std::function<void(data_t &, const id_t)>;

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
    template <typename id_t, typename data_t, typename unique_field_t>
    class Store
    {
    public:
        Store(id_t defaultId, id_t maxElement,
              GetUniqueFieldFunc<data_t, unique_field_t> getUniqueFieldFunc = nullptr)
            : m_defaultId(defaultId),
              m_currentId(defaultId),
              m_max(maxElement),
              m_getUniqueFieldFunc(getUniqueFieldFunc),
              m_store({}),
              m_uniqueFieldToId({}),
              m_freedIds({})
        {
        }

        ~Store()
        {
            m_store.Clear();
            m_uniqueFieldToId.Clear();
            m_freedIds.Clear();
        }

        /**
         * When new object, it will return the id of the object.
         *      which can be a completely new id or a reused id (if
         *      the unique_field is the same).
         */
        id_t Add(data_t data)
        {
            if (m_store.Length() >= m_max)
            {
                NTT_ENGINE_WARN("The store is full, cannot add more elements");
                return m_defaultId;
            }

            unique_field_t uniqueField = m_getUniqueFieldFunc(data);

            if (m_uniqueFieldToId.Contains(uniqueField))
            {
                return m_uniqueFieldToId.Get(uniqueField);
            }

            auto dataPtr = CreateRef<data_t>(data);
            m_uniqueFieldToId.Insert(uniqueField, m_currentId);

            if (m_freedIds.Length() > 0)
            {
                auto id = m_freedIds[0];
                m_store.Set(id, dataPtr);

                return id;
            }
            else
            {
                m_store.Add(dataPtr);
                return m_currentId++;
            }
        }

        /**
         * Query the object with the id.
         *      if the object is not found, it will return nullptr.
         */
        Ref<data_t> Get(id_t id)
        {
            if (!Contains(id))
            {
                NTT_ENGINE_WARN("The id is not existed in the store");
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
        u8 Contains(data_t data)
        {
            unique_field_t uniqueField = m_getUniqueFieldFunc(data);
            return m_uniqueFieldToId.Contains(uniqueField);
        }

        /**
         * Check if the unique field is already stored in the store.
         */
        u8 ContainsUnique(unique_field_t uniqueField)
        {
            return m_uniqueFieldToId.Contains(uniqueField);
        }

        /**
         * Remove the object with the id. Then that id
         *      can be reused, if the id is not added,
         *      then nothing happens
         */
        void Release(id_t id)
        {
            Ref<data_t> data = Get(id);

            if (data != nullptr)
            {
                m_uniqueFieldToId.Remove(m_getUniqueFieldFunc(*data));
                m_store.Set(id, nullptr);
                data.reset();

                for (auto i = 0; i < m_freedIds.Length(); i++)
                {
                    if (m_freedIds[i] > id)
                    {
                        m_freedIds.Add(i, id);
                        return;
                    }
                }

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
                    availableIds.Add(i);
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
                    func(*m_store[i], i);
                }
            }
        }

    private:
        id_t m_defaultId;
        id_t m_currentId;
        id_t m_max;
        List<Ref<data_t>> m_store;
        GetUniqueFieldFunc<data_t, unique_field_t> m_getUniqueFieldFunc;
        Dictionary<unique_field_t, id_t> m_uniqueFieldToId;
        List<id_t> m_freedIds;
    };
} // namespace ntt::dev::store
