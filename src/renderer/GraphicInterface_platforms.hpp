#pragma once
#include <NTTEngine/defines.hpp>

#ifdef PLATFORM_WINDOWS
#include <raylib.h>
#define TEXTURE_2D Texture2D
#define LOAD_TEXTURE(path) ::LoadTexture(path.RawString().c_str())
#define UNLOAD_TEXTURE(texture) ::UnloadTexture(texture)
#define IS_LOADED_SUCCESS(texture) (texture.id == 0)
#define DRAW_TEXTURE(texture, fx, fy, fw, fh, tx, ty, tw, th, rotate) \
    ::DrawTexturePro(texture,                                         \
                     ::Rectangle{fx, fy, fw, fh},                     \
                     ::Rectangle{tx, ty, tw, th},                     \
                     ::Vector2{tw / 2, th / 2},                       \
                     rotate,                                          \
                     ::WHITE)
#else
#error "The platform is not supported"
#endif