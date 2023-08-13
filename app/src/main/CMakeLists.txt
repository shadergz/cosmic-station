
cmake_minimum_required(VERSION 3.22.1)

project(ZenithEmu LANGUAGES CXX ASM VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# zenith: PS2 core emulator native library
add_library(zenith MODULE)
add_library(addons SHARED)

target_compile_options(zenith PRIVATE
        -Wall -Werror -Wreorder -Wno-unknown-pragmas -Wno-missing-braces
        -march=armv8-a -frtti -fvisibility=hidden -funroll-loops
        $<$<CONFIG:Release>:
            -Ofast;-DNDEBUG;-ffunction-sections;-fdata-sections;
            -fno-stack-protector;-fomit-frame-pointer;-flto=full>
        $<$<CONFIG:Debug>:
            -glldb;-fno-omit-frame-pointer;-O0>)

target_link_options(zenith PRIVATE $<$<CONFIG:Release>:-s;-Wl,--gc-sections;>)
target_link_libraries(zenith addons)

set(ZENITH_DIR ${CMAKE_SOURCE_DIR}/cpp/zenith)
set(ADDONS_DIR ${CMAKE_SOURCE_DIR}/cpp/addons)
set(ZENITH_MISC_DIR ${CMAKE_SOURCE_DIR}/cpp)

target_include_directories(zenith PRIVATE ${ZENITH_MISC_DIR} ${ZENITH_DIR})
target_sources(zenith PRIVATE
        ${ZENITH_DIR}/app.cpp
        ${ZENITH_DIR}/readable_log.cpp
        ${ZENITH_DIR}/os/mach_state.cpp
        ${ZENITH_DIR}/java/device_res.cpp
        ${ZENITH_MISC_DIR}/driver_chk.cpp
        ${ZENITH_MISC_DIR}/jvm.cpp)
target_sources(addons PRIVATE
        ${ADDONS_DIR}/compile_this.cpp)

