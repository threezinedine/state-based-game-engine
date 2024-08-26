cmake_minimum_required(VERSION 3.0)

project(NTTEngine)

add_subdirectory(vendors)

add_executable(
    Example
    main.cpp
)

target_include_directories(
    Example
    PUBLIC
    .
)

target_link_libraries(
    Example
    PUBLIC
    raylib
)