cmake_minimum_required(VERSION 3.28)
project(Chippy8 CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

add_executable(Chippy8
        main.cpp
                )
target_link_libraries(Chippy8 ${SDL3_LIBRARIES})