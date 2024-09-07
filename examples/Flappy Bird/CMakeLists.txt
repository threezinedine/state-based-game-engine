project(
    FlappyBird
)

file(
    GLOB_RECURSE
    SOURCES
    *.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(
    ${PROJECT_NAME}
    NTTEngine
)