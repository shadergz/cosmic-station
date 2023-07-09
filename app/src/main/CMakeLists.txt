
cmake_minimum_required(VERSION 3.22.1)

project(Oblivion VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# oblivion: PS2 core emulator native library
add_library(oblivion MODULE)

target_link_libraries(oblivion )

set(OBLIVION_DIR ${CMAKE_SOURCE_DIR}/cpp/oblivion)
set(OBLIVION_MISC_DIR ${CMAKE_SOURCE_DIR}/cpp)

target_sources(oblivion PRIVATE
        ${OBLIVION_DIR}/vm.cpp
        ${OBLIVION_MISC_DIR}/driver_chk.cpp
        ${OBLIVION_MISC_DIR}/jvm.cpp
        )

