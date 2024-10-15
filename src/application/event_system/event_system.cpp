#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    /**
     * Need this for deleting the callback
     *      by storing the callback with
     *      the event id.
     */
    struct EventCallbackContext
    {
        event_id_t id;
        EventCallback callback;
    };

    namespace
    {
        // Number representing the total of callback events
        //      which are registered (includes the unregistered one)
        //      The next event id will be the current id
        event_id_t s_currentId = 0;

        // The dictionary which stores the event code and the
        //     list of callbacks which are registered to the event
        Dictionary<event_code_t, List<EventCallbackContext>> s_eventCallbacks;
    } // namespace

    void EventInit()
    {
        PROFILE_FUNCTION();
        s_eventCallbacks = Dictionary<event_code_t, List<EventCallbackContext>>();
    }

    event_id_t RegisterEvent(event_code_t event_code, EventCallback callback)
    {
        PROFILE_FUNCTION();
        // TODO: Handle problem when the event code is reach the maximum
        s_eventCallbacks[event_code].push_back({s_currentId, callback});
        return s_currentId++;
    }

    void UnregisterEvent(event_id_t event_id)
    {
        PROFILE_FUNCTION();
        for (auto &event : s_eventCallbacks)
        {
            event.second.RemoveItem(
                {event_id},
                [](const EventCallbackContext &a, const EventCallbackContext &b)
                { return a.id == b.id; });
        }
    }

    void TriggerEvent(event_code_t event_code, void *sender, const EventContext &context)
    {
        PROFILE_FUNCTION();
        if (!s_eventCallbacks.Contains(event_code))
        {
            return;
        }

        auto callbacks = s_eventCallbacks[event_code];

        callbacks.ForEach([&](const EventCallbackContext &callbackCxt, ...)
                          { callbackCxt.callback(event_code, sender, context); });
    }

    void EventShutdown()
    {
        PROFILE_FUNCTION();
        s_eventCallbacks.clear();
    }
} // namespace ntt
