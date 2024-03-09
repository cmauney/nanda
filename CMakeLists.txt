cmake_minimum_required(VERSION 3.25)

project(nanda)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(fmt REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
endif()


add_subdirectory(libs)
add_subdirectory(apps)


