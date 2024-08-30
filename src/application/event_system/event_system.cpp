#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>

namespace ntt::event
{
    using namespace log;

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
        Dictionary<EventCode, List<EventCallbackContext>> s_eventCallbacks;
    } // namespace

    event_id_t RegisterEvent(EventCode event_code, EventCallback callback)
    {
        // TODO: Handle problem when the event code is reach the maximum
        s_eventCallbacks[event_code].Add({s_currentId, callback});
        return s_currentId++;
    }

    void UnregisterEvent(event_id_t event_id)
    {
        s_eventCallbacks.ForEach(
            [&](const EventCode &event_code, List<EventCallbackContext> &callbacks)
            {
                callbacks.RemoveItem({event_id}, [](const EventCallbackContext &a, const EventCallbackContext &b)
                                     { return a.id == b.id; });
            });
    }

    void TriggerEvent(EventCode event_code, void *sender, const EventContext &context)
    {
        auto callbacks = s_eventCallbacks[event_code];

        callbacks.ForEach([&](const EventCallbackContext &callbackCxt, ...)
                          { callbackCxt.callback(event_code, sender, context); });
    }
} // namespace ntt::event
