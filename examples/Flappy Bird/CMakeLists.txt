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
    PUBLIC
    NTTEngine
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)