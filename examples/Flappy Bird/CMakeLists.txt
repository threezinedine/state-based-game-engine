project(
    FlappyBird
)

file(
    GLOB_RECURSE
    SOURCES
    *.cpp
)

file (
    GLOB_RECURSE
    SCRIPTS
    scripts/*.cpp
)

list(REMOVE_ITEM SOURCES ${SCRIPTS})

foreach(SOURCE ${SOURCES})
    message(STATUS "SOURCE: ${SOURCE}") 
endforeach()

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


add_executable(
    ${PROJECT_NAME}_Editor
    ${SOURCES}
)

target_link_libraries(
    ${PROJECT_NAME}_Editor
    PUBLIC
    NTTEngine
)

target_include_directories(
    ${PROJECT_NAME}_Editor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(
    ${PROJECT_NAME}_Editor
    PRIVATE
    _EDITOR
)