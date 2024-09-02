#pragma once
#include "dev.hpp"
#include <functional>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>

namespace ntt::dev::store
{
    template <typename data_t, typename unique_field_t>
    using GetUniqueFieldFunc = std::function<unique_field_t(const data_t &)>;

    /**
     * Provide a generic way to storing resources
     *      object in the engine.
     *
     * @tparam id_t The type of the id used to identify the object.
     * @tparam data_t The type of the object to store.
     * @tparam unique_field_t The additional field to ensure the uniqueness of the object.
     *      (ex: a file should only be loaded once)
     */
    template <typename id_t, typename data_t, typename unique_field_t>
    class Store
    {
    public:
        Store(id_t defaultId, id_t max_element,
              GetUniqueFieldFunc<data_t, unique_field_t> get_unique_field_func = nullptr);
        ~Store();

        /**
         * When new object, it will return the id of the object.
         *      which can be a completely new id or a reused id (if
         *      the unique_field is the same).
         */
        id_t Add(data_t data);

        /**
         * Query the object with the id.
         *      if the object is not found, it will return nullptr.
         */
        Ref<data_t> Get(id_t id);

        /**
         * Check if the object is currently available in the store.
         */
        u8 Contains(id_t id);

        /**
         * Check if the unique field is already stored in the store.
         */
        u8 Contains(data_t data);

        /**
         * Remove the object with the id. Then that id
         *      can be reused, if the id is not added,
         *      then nothing happens
         */
        void Release(id_t id);

        /**
         * Get all ids which their objects are currently stored.
         */
        List<id_t> GetAvailableIds();

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
