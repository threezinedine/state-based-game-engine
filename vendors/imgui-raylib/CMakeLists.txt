project(
    ImGui_Raylib
)

file(
    GLOB_RECURSE
    IMGUI_RAYLIB_SOURCES
    src/*.cpp
)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_NO_VERBOSE ON)

add_library(
    ImGui_Raylib
    STATIC
    ${IMGUI_RAYLIB_SOURCES}
)

target_include_directories(
    ImGui_Raylib
    PUBLIC
    include
)

target_link_libraries(
    ImGui_Raylib
    PUBLIC
    raylib
)