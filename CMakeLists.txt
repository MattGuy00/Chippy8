cmake_minimum_required(VERSION 3.28)
project(Chippy8 C)

set(CMAKE_C_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Chippy8
        main.c)
target_link_libraries(Chippy8 ${SDL2_LIBRARIES})