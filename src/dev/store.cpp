#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/dev/store.hpp>
#include "store_conf_t.hpp"
#include "../audio/audio_info.hpp"

namespace ntt::dev::store
{
    using namespace audio;

#define STORE(...) template <typename id_t, typename data_t, typename unique_field_t> \
__VA_ARGS__ Store<id_t, data_t, unique_field_t>

    STORE()::Store(id_t defaultId,
                   id_t maxElement,
                   GetUniqueFieldFunc<data_t, unique_field_t> getUniqueFieldFunc)
        : m_defaultId(defaultId),
          m_currentId(defaultId),
          m_max(maxElement),
          m_getUniqueFieldFunc(getUniqueFieldFunc),
          m_store({}),
          m_uniqueFieldToId({}),
          m_freedIds({})
    {
    }

    STORE()::~Store()
    {
        m_store.Clear();
        m_uniqueFieldToId.Clear();
        m_freedIds.Clear();
    }

    STORE(id_t)::Add(data_t data)
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

    STORE(Ref<data_t>)::Get(id_t id)
    {
        if (!Contains(id))
        {
            NTT_ENGINE_WARN("The id is not existed in the store");
            return nullptr;
        }

        return m_store[id];
    }

    STORE(u8)::Contains(id_t id)
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

    STORE(u8)::Contains(data_t data)
    {
        unique_field_t uniqueField = m_getUniqueFieldFunc(data);
        return m_uniqueFieldToId.Contains(uniqueField);
    }

    STORE(u8)::ContainsUnique(unique_field_t uniqueField)
    {
        return m_uniqueFieldToId.Contains(uniqueField);
    }

    STORE(void)::Release(id_t id)
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

    STORE(List<id_t>)::GetAvailableIds()
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

    template class Store<u32, TestObj, u32>;
    template class Store<audio_id_t, AudioInfo, String>;
} // namespace ntt::dev::store
