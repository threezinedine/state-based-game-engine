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
namespace ntt
{
    // For futher increase the number of event
    //      registeration, the event_id_t can be
    //      changed to u32 or u64
    using event_id_t = u16;

    /**
     * The data frame which will be passed to all the
     *      subscribers when the event is triggered
     *
     * It has length 16 bytes and can be used to store
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

#define NTT_ENTITY_CREATED 0x07   ///< When an entity is created
#define NTT_ENTITY_DESTROYED 0x08 ///< When an entity is destroyed

#define NTT_END_FRAME 0x0C ///< When an update frame is finished (trigger
                           ///< at the end of the game loop)

#define NTT_SCENE_CHANGED 0x0E ///< When the scene is changed

#define NTT_RESOURCE_LOADED 0x0F   ///< When the resource is loaded
#define NTT_RESOURCE_UNLOADED 0x10 ///< When the resource is unloaded

#define NTT_FILE_CHANGED 0x11 ///< When the file is changed
#define NTT_FILE_UPDATED 0x12 ///< When the file is updated

#define NTT_SCRIPT_FILE_CHANGED 0x13 ///< When the script file is changed

#define NTT_EDITOR_START_GAME 0x14 ///< When the editor is started
#define NTT_EDITOR_STOP_GAME 0x16  ///< When the editor is stopped

#define NTT_LAYER_CHANGED 0x0D         ///< When the layer is changed
#define NTT_DEFINED_LAYER_CHANGED 0x17 ///< When the defined layer is changed (not visible layer)

#define NTT_EDITOR_CLEAR_CHOSEN_ENTITY 0x18 ///< When the entity is cleared in the editor
#define NTT_EDITOR_CHOOSE_ENTITY 0x19       ///< When the entity is chosen in the editor
#define NTT_EDITOR_APPEND_ENTITY 0x1A       ///< When the entity is appended in the editor

#define NTT_EDITOR_SELECTED_RESIZE 0x1C         ///< When the entity need to be resized
#define NTT_EDITOR_SELECTED_RESIZE_REQUEST 0x1D ///< When the entity has request to be resized

#define NTT_EDITOR_SELECTED_ROTATE 0x1E         ///< When the entity need to be rotated
#define NTT_EDITOR_SELECTED_ROTATE_REQUEST 0x1F ///< When the entity has request to be rotated

#define NTT_EDITOR_TRANSFORM_CHANGED 0x20         ///< When the transform of the entity is changed
#define NTT_EDITOR_TRANSFORM_CHANGED_REQUEST 0x21 ///< When the transform of the entity is changed

#define NTT_EDITOR_TOOL_TYPE_CHANGED 0x22 ///< When the tool type is changed

#define NTT_APPLICATION_RESET 0x23 ///< When the application is reset

#define NTT_EDITOR_SAVE_CONFIG 0x24    ///< When the editor configuration is saved
#define NTT_EDITOR_CREATE_PROJECT 0x25 ///< When the editor project is created
#define NTT_EDITOR_PROJECT_LOADED 0x26 ///< When the editor project is loaded

#define NTT_HISTORY_EMPTY 0x27     ///< When the history stack is empty
#define NTT_HISTORY_NOT_EMPTY 0x28 ///< When the history stack is not empty

#define NTT_EDITOR_CREATE_NEW_SCENE 0x29 ///< When the new scene is created
#define NTT_EDITOR_SAVE_PROJECT 0x2A     ///< When the editor project is saved
#define NTT_EDITOR_OPEN_SCENE 0x2B       ///< When the scene is opened
#define NTT_EDITOR_SAVE_SCENE 0x2C       ///< When the scene is saved

#define NTT_EDITOR_COPY_TEXTURE_GRID 0x2D ///< When the texture grid is copied

#define NTT_EDITOR_SELECTED_MOVE 0x2E         ///< When the entity need to be moved
#define NTT_EDITOR_SELECTED_MOVE_REQUEST 0x2F ///< When the entity has request to be moved

    // TODO: Change the default event < 255 and the user's event >= 255

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
     * Starting the event bus with needed information and logic
     */
    void EventInit();

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

    /**
     * Destroy the event bus and free the memory
     */
    void EventShutdown();
} // namespace ntt
