cmake_minimum_required(VERSION 3.20)
project(test)

include_directories(.)
set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out/${MY_BUILD_NAME}/test/)
file(INSTALL run_tests DESTINATION ${CMAKE_SOURCE_DIR}/out/${MY_BUILD_NAME}/test/)
add_subdirectory(app)
add_subdirectory(lib)
