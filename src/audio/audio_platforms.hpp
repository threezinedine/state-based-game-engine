#pragma once

#ifdef PLATFORM_WINDOWS
#include <raylib.h>

#define INIT_DEVICE() InitAudioDevice()
#define SOUND Sound
#define LOAD_SOUND(sound, path) SOUND sound = LoadSound(path.RawString().c_str())
#define IS_LOADED_SUCCESS(sound) sound.stream.buffer != nullptr
#define PLAY_SOUND(sound) PlaySound(sound)
#define IS_SOUND_PLAYING(sound) IsSoundPlaying(sound)
#define PAUSE_SOUND(sound) PauseSound(sound)
#define RESUME_SOUND(sound) ResumeSound(sound)
#define STOP_SOUND(sound) StopSound(sound)
#define UNLOAD_SOUND(sound) UnloadSound(sound)

#else

#define INIT_DEVICE()
#define SOUND
#define LOAD_SOUND(sound, path)
#define IS_LOADED_SUCCESS(sound)
#define PLAY_SOUND(sound)
#define IS_SOUND_PLAYING(sound)
#define PAUSE_SOUND(sound)
#define RESUME_SOUND(sound)
#define STOP_SOUND(sound)
#define UNLOAD_SOUND(sound)

#endif