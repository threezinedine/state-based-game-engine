cmake_minimum_required(VERSION 3.0)

project(NTTEngine)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    add_definitions(-D_DEBUG)
    add_compile_definitions(_DEBUG)
endif()

# option(NTT_BUILD_SHARED "Build shared library" ON)
option(NTT_BUILD_SHARED "Build shared library" OFF)

# TODO: fix later
# add_compile_definitions(_WIN32)
# add_compile_definitions(_MSC_VER)

file(
    GLOB
    VENDORS_SOURCES
    vendors/*
)

set(
    IGNORED_FOLDERS
)

list(
    REMOVE_ITEM
    VENDORS_SOURCES
    ${IGNORED_FOLDERS}
)

foreach(VENDORS_SOURCE ${VENDORS_SOURCES})
    if(IS_DIRECTORY ${VENDORS_SOURCE})
        add_subdirectory(${VENDORS_SOURCE})
    endif()
endforeach()

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
    # SHARED
    STATIC
    ${SOURCES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ./include
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    vendors/fmt/include
    # vendors/nlohmann_json/include
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    raylib
    nlohmann_json
)

if (NTT_BUILD_SHARED)
    add_compile_definitions(NTTENGINE_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE memplumber)
endif()

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