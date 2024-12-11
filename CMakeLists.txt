cmake_minimum_required(VERSION 3.25)
project(Chippy8 CXX)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

add_executable(Chippy8
        main.cpp
		Chip8_Emu.cpp)
target_link_libraries(Chippy8 PUBLIC ${SDL3_LIBRARIES})
