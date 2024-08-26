cmake_minimum_required(VERSION 3.0)

project(NTTEngine)

add_subdirectory(vendors)

file(
    GLOB_RECURSE
    SOURCES
    src/*.cpp
)

add_executable(
    Example
    ${SOURCES}
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

file(
    GLOB_RECURSE
    TEST_SOURCES
    test/*.cpp
)

add_executable(
    ${PROJECT_NAME}_Test
    ${TEST_SOURCES}
)

target_include_directories(
    ${PROJECT_NAME}_Test
    PUBLIC
    .
)

target_link_libraries(
    ${PROJECT_NAME}_Test
    PUBLIC
    gtest 
    gmock
)