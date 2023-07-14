
cmake_minimum_required(VERSION 3.22.1)

project(ZenithEmu VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# zenith: PS2 core emulator native library
add_library(zenith MODULE)

target_link_libraries(zenith )

set(ZENITH_DIR ${CMAKE_SOURCE_DIR}/cpp/zenith)
set(ZENITH_MISC_DIR ${CMAKE_SOURCE_DIR}/cpp)

target_sources(zenith PRIVATE
        ${ZENITH_DIR}/vm.cpp
        ${ZENITH_MISC_DIR}/driver_chk.cpp
        ${ZENITH_MISC_DIR}/jvm.cpp)

