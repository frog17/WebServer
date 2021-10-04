# CMakeList.txt : CMake project for WebServer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR}/_include)
#include_directories(${PROJECT_SOURCE_DIR}/server)

add_executable (WebServer "main.cpp")

add_subdirectory(server)
add_subdirectory(log)
add_subdirectory(base)

target_link_libraries(WebServer server)
target_link_libraries(WebServer xlog)
target_link_libraries(WebServer base)

# Add source to this project's executable.

