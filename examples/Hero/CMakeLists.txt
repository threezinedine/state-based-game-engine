project(
    Hero
)

file(
    GLOB_RECURSE
    SOURCES
    *.cpp
)

add_executable(
    Hero
    ${SOURCES}
)

target_link_libraries(
    Hero
    NTTEngine
)