#pragma once
#include <NTTEngine/defines.hpp>
#include <functional>

/**
 * The event bus of the engine
 *      every module in this engine
 *      will communicate through this
 *      system (except the logging module will
 *      be used directly)
 *
 * Every one who subscribes to a certain event (reprsented by the event code)
 *      will be notified when the event is triggered (by any one)
 *      and the data context will be passed to the subscriber
 */
namespace ntt::event
{
    // For futher increase the number of event
    //      registeration, the event_id_t can be
    //      changed to u32 or u64
    using event_id_t = u16;

    /**
     * The data frame which will be passed to all the
     *      subscribers when the event is triggered
     *
     * It has length 128 bytes and can be used to store
     *      multiple data types
     */
    union EventContext
    {
        u64 u64_data[2];
        u32 u32_data[4];
        u16 u16_data[8];
        u8 u8_data[16];

        f64 f64_data[2];
        f32 f32_data[4];

        i64 i64_data[2];
        i32 i32_data[4];
        i16 i16_data[8];
        i8 i8_data[16];

        b8 b8_data[16];
        b32 b32_data[4];
    };

    using event_code_t = u8;

    /**
     * All the event codes that this engine supports
     */
#define NTT_EVENT_TEST 0x00         ///< Test event code which is used for testing purpose (can be used
                                    ///<      as reversed event code)
#define NTT_EVENT_KEY_PRESSED 0x01  ///< When a keyboard button or mouse button is pressed
#define NTT_EVENT_KEY_RELEASED 0x02 ///< When a keyboard button or mouse button is released

#define NTT_EVENT_WINDOW_RESIZED 0x03 ///< When the window is resized

#define NTT_EVENT_AUDIO_FINISHED 0x04 ///< When the audio is finished playing

#define NTT_EVENT_MOUSE_PRESS 0x05   ///< When the mouse button is pressed
#define NTT_EVENT_MOUSE_RELEASE 0x06 ///< When the mouse button is released

    /**
     * Can add more event code here
     *      without changing any other
     *      code in the engine
     */

    /**
     * The type of the callback function which
     *      is used in the event bus system
     */
    using EventCallback = std::function<void(const event_code_t, void *, const EventContext &)>;

    /**
     * When any callback is registered to the event bus
     *      it will be called each time the event code
     *      is triggered.
     *
     * @param event_code The code of the event
     * @param callback The callback function must be the `EventCallback`
     *
     * @return The id of the event, which can be used
     *      to unregister the event (can be ignored
     *      if the event is not needed to be unregistered)
     */
    event_id_t RegisterEvent(event_code_t event_code,
                             EventCallback callback);

    /**
     * Remove the event from the event bus
     *      then the callback of the event
     *      with given id will not be called
     *      anymore when the event is triggered
     *
     * @param event_id The id of the event which
     *      is obtained when the event is registered.
     *      If the event is is not valid (not registered
     *      or already unregistered) then nothing
     *      happens, otherwise a warning log will be
     *      printed.
     */
    void UnregisterEvent(event_id_t event_id);

    /**
     * Pass the event with data context to the event bus
     */
    void TriggerEvent(event_code_t event_code,
                      void *sender = nullptr,
                      const EventContext &context = {});
} // namespace ntt::event
