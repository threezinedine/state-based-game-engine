cmake_minimum_required(VERSION 3.0)

project(NTTEngine)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    add_compile_definitions(_DEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(STATUS "C++ compiler is Clang++")
        # Add specific C++ compiler flags for Clang++
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    else()
        message(STATUS "C++ compiler is not Clang")
    endif()
endif()

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