cmake_minimum_required(VERSION 3.20)
project(FiffFileExplorer)

set(CMAKE_CXX_STANDARD 14)

if(NOT DEFINED ${MY_BUILD_NAME})
    set(MY_BUILD_NAME ${CMAKE_BUILD_TYPE})
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-g -O0 -ftest-coverage -fprofile-arcs)
    add_link_options(--coverage)
    add_subdirectory(test)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
