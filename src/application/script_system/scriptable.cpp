#include <NTTEngine/application/script_system/scriptable.hpp>

namespace ntt::script
{
    JSON &Scriptable::GetData()
    {
        auto data = GetComponent<DataComponent>();
        return data->data;
    }

    void Scriptable::Subscribe(event_code_t eventCode, EventCallback callback)
    {
        auto id = RegisterEvent(eventCode, callback);
        events.push_back(id);
    }

    void Scriptable::Delete()
    {
        if (m_deleted)
        {
            return;
        }
        m_deleted = TRUE;

        for (auto id : events)
        {
            UnregisterEvent(id);
        }
        OnExitImpl();
        ECSDeleteEntity(entity_id);
    }
} // namespace ntt
