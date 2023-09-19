
cmake_minimum_required(VERSION 3.22.1)

project(ZenithEmu LANGUAGES CXX ASM VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ASM_FLAGS )

set(CMAKE_CXX_FLAGS "-Wall -march=armv8-a -frtti -fvisibility=hidden")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast -fno-stack-protector -fomit-frame-pointer -flto=full")
set(CMAKE_CXX_FLAGS_DEBUG "-glldb -fstack-protector-strong -O0")

set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-z,noexecstack")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "-s -Wl,--gc-sections")

# zenith: PS2 core emulator native library
add_library(zenith MODULE)
add_library(addons SHARED)

set(ZENITH_DIR ${CMAKE_SOURCE_DIR}/cpp/zenith)
set(ADDONS_DIR ${CMAKE_SOURCE_DIR}/cpp/addons)
set(ZENITH_MISC_DIR ${CMAKE_SOURCE_DIR}/cpp)

target_include_directories(zenith PRIVATE ${ZENITH_MISC_DIR} ${ZENITH_DIR})

target_sources(zenith PRIVATE
    ${ZENITH_DIR}/app.cpp
    ${ZENITH_DIR}/paper_log.cpp
    ${ZENITH_DIR}/eeiv/ee_engine.cpp
    ${ZENITH_DIR}/eeiv/cop0.cpp
    ${ZENITH_DIR}/eeiv/mmu_tlb.cpp
    ${ZENITH_DIR}/eeiv/casper/mipsiv_full_interpreter.cpp
    ${ZENITH_DIR}/iop/iop_core.cpp
    ${ZENITH_DIR}/os/mach_state.cpp
    ${ZENITH_DIR}/java/device_res.cpp
    ${ZENITH_DIR}/console/devices_ctrl.cpp
    ${ZENITH_MISC_DIR}/driver_chk.cpp
    ${ZENITH_MISC_DIR}/jvm.cpp)

target_sources(addons PRIVATE
    ${ADDONS_DIR}/compile_this.cpp)

target_compile_options(zenith PRIVATE -Wreorder -Wconversion -Wformat-security -Wshadow)

target_link_libraries(zenith PRIVATE addons)

