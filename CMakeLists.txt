cmake_minimum_required(VERSION 3.0)

project(NTTEngine)

add_subdirectory(vendors)

file(
    GLOB_RECURSE
    SOURCES
    src/*.cpp
)

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
    ${PROJECT_NAME}
    gtest 
    gmock
)

add_subdirectory(examples)