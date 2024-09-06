#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/resources/resource_common.h>

/**
 * Provide all functionalities related to audio.
 *
 * The audio lifecycle is:
 *      - Load audio file
 *      - Play audio
 *      - Pause audio (has the event effect)
 *      - Resume audio (has the event effect)
 *      - Stop audio (has the event effect)
 *      - Unload audio file
 */
namespace ntt::audio
{
    /**
     * Start the audio system. If not be called, other functions
     *      will call this function automatically.
     */
    void AudioInit();

    /**
     * Load audio file from the given path (current support only
     *      .wav file).
     *
     * @param path The path to the audio file, if the audio file
     *      is not exist or not supported, a warning is logged
     *      and the return is the default audio (0)
     *
     * @return The audio id, if the path is already loaded, then
     *      the previous audio id is returned else a new audio id
     *      is returned.
     */
    resource_id_t LoadAudio(const String &path);

    /**
     * Run the audio with the given audio id.
     *
     * @param audio_id The audio id to run, if the audio id is not
     *      existed or unloaded, the warning is logged and nothing
     *      is played.
     */
    void PlayAudio(resource_id_t audio_id);

    /**
     * Stop the audio and reset the progress to the beginning.
     *
     * @param audio_id The audio id to stop, if the audio id is not
     *      existed or unloaded, the warning is logged and nothing is
     *      stopped.
     *      With the paused audio, the state is also reset to the beginning.
     */
    void StopAudio(resource_id_t audio_id);

    /**
     * Unload the audio file from the given audio id.
     *
     * @param audio_id The audio id to unload, if the audio id
     *      is not existed or unloaded, the warning is logged.
     *      The default audio (0) is not allowed to unload. It will
     *      be unloaded automatically when the application is closed.
     *      (Print warning if the default audio is tried to unload)
     */
    void UnloadAudio(resource_id_t audio_id);

    /**
     * Use in the game loop only. Use must not call this function
     *
     * @param delta The time between the last frame and the current
     */
    void AudioUpdate(f32 delta);

    /**
     * Stop the audio system. Must be called when the application
     *      is closed.
     */
    void AudioShutdown();
} // namespace ntt::audio
