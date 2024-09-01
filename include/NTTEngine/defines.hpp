#pragma once

/**
 * Change primitive types to more size-specific types
 *  to ensure the size of the types in different platforms.
 *
 * Each type must be validated with the size of the type
 */

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

using b8 = bool;
using b32 = u32;

static_assert(sizeof(u8) == 1, "u8 is not 1 byte");
static_assert(sizeof(u16) == 2, "u16 is not 2 bytes");
static_assert(sizeof(u32) == 4, "u32 is not 4 bytes");
static_assert(sizeof(u64) == 8, "u64 is not 8 bytes");

static_assert(sizeof(i8) == 1, "i8 is not 1 byte");
static_assert(sizeof(i16) == 2, "i16 is not 2 bytes");
static_assert(sizeof(i32) == 4, "i32 is not 4 bytes");
static_assert(sizeof(i64) == 8, "i64 is not 8 bytes");

static_assert(sizeof(f32) == 4, "f32 is not 4 bytes");
static_assert(sizeof(f64) == 8, "f64 is not 8 bytes");

static_assert(sizeof(b8) == 1, "b8 is not 1 byte");
static_assert(sizeof(b32) == 4, "b32 is not 4 bytes");

#define TRUE 1
#define FALSE 0

// // TODO: Currently only support Windows platform
// #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PLATFORM_WINDOWS
// #else
// #error "This library only supports Windows"
// #endif
