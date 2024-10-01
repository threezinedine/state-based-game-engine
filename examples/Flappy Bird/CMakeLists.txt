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
    # "C:/Users/Acer/Games Dev/state-based-game-engine/build/libNTTEngine.dll.a"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)