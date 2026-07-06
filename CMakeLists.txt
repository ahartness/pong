cmake_minimum_required(VERSION 3.16)
project(pong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(pong src/main.cpp)
target_link_libraries(pong raylib)

# Linux-specific libs raylib often needs
if (UNIX AND NOT APPLE)
  target_link_libraries(pong m pthread dl rt X11)
endIf()
