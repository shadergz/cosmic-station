
cmake_minimum_required(VERSION 3.22.1)

project(ZenithEmu LANGUAGES CXX ASM VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# zenith: PS2 core emulator native library
add_library(zenith MODULE)
add_library(addons SHARED)

add_compile_options(-Wall -Werror -march=armv8-a -frtti -fvisibility=hidden
    "$<$<CONFIG:Release>: -Ofast; -DNDEBUG; -flto=full; -fno-stack-protector; -fomit-frame-pointer;>"
    "$<$<CONFIG:Debug>: -glldb; -fno-omit-frame-pointer; -O0>")

target_compile_options(zenith PRIVATE -Wreorder -Wconversion -Wformat-security -Wshadow
    -Wno-unused-private-field)

add_link_options(-Wl,-z,noexecstack "$<$<CONFIG:Release>:-s; -Wl,--gc-sections;>")

target_link_libraries(zenith addons)
set(ZENITH_DIR ${CMAKE_SOURCE_DIR}/cpp/zenith)
set(ADDONS_DIR ${CMAKE_SOURCE_DIR}/cpp/addons)
set(ZENITH_MISC_DIR ${CMAKE_SOURCE_DIR}/cpp)

target_include_directories(zenith PRIVATE ${ZENITH_MISC_DIR} ${ZENITH_DIR})

target_sources(zenith PRIVATE
    ${ZENITH_DIR}/app.cpp
    ${ZENITH_DIR}/paper_log.cpp
    ${ZENITH_DIR}/eeiv/ee_engine.cpp
    ${ZENITH_DIR}/os/mach_state.cpp
    ${ZENITH_DIR}/os/host_memory.cpp
    ${ZENITH_DIR}/java/device_res.cpp
    ${ZENITH_MISC_DIR}/driver_chk.cpp
    ${ZENITH_MISC_DIR}/jvm.cpp)

target_sources(addons PRIVATE
    ${ADDONS_DIR}/compile_this.cpp)

