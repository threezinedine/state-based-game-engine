cmake_minimum_required(VERSION 3.0)

project(NTTEngine)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    add_definitions(-D_DEBUG)
    add_compile_definitions(_DEBUG)
endif()

# TODO: fix later
# add_compile_definitions(_WIN32)
# add_compile_definitions(_MSC_VER)

add_subdirectory(vendors)

file(
    GLOB_RECURSE
    SOURCES
    src/*.cpp
)

set (TEST_SOURCES)

foreach(SOURCE ${SOURCES})
    if (SOURCE MATCHES ".*__tests__.*")
        list(APPEND TEST_SOURCES ${SOURCE})
        list(REMOVE_ITEM SOURCES ${SOURCE})
    endif()
endforeach()


add_library(
    ${PROJECT_NAME}
    STATIC
    ${SOURCES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ./include
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    raylib
    nlohmann_json
    memplumber 
    fmt
)

add_executable(
    ${PROJECT_NAME}_Test
    ${TEST_SOURCES}
    _test.cpp
)

target_include_directories(
    ${PROJECT_NAME}_Test
    PUBLIC
    ./include
    ./src
    ./
)

target_link_libraries(
    ${PROJECT_NAME}_Test
    PUBLIC
    ${PROJECT_NAME}
    gtest 
    gmock
)

add_compile_definitions(BUILD_GAME)
add_subdirectory(examples)