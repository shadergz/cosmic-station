
cmake_minimum_required(VERSION 3.22.1)

project(ZenithEmu LANGUAGES CXX ASM VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Wall -march=armv8-a -fno-rtti -fvisibility=hidden")
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
    ${ZENITH_DIR}/logger.cpp
    ${ZENITH_DIR}/eeiv/ee_engine.cpp
    ${ZENITH_DIR}/eeiv/ee_iov0.cpp
    ${ZENITH_DIR}/eeiv/cop0.cpp
    ${ZENITH_DIR}/eeiv/mmu_tlb.cpp
    ${ZENITH_DIR}/kernel/hle.cpp
    ${ZENITH_DIR}/kernel/group.cpp
    ${ZENITH_DIR}/eeiv/fuji/mipsiv_full_interpreter.cpp
    ${ZENITH_DIR}/iop/iop_core.cpp
    ${ZENITH_DIR}/os/system_state.cpp
    ${ZENITH_DIR}/java/device_handler.cpp
    ${ZENITH_DIR}/java/jclasses.cpp
    ${ZENITH_DIR}/console/device_ctrl.cpp
    ${ZENITH_DIR}/console/emu_vm.cpp
    ${ZENITH_MISC_DIR}/driver_glvk_chk.cpp
    ${ZENITH_MISC_DIR}/jvm_comm.cpp)

add_subdirectory(${ADDONS_DIR}/fmt EXCLUDE_FROM_ALL)

target_sources(addons PRIVATE
    ${ADDONS_DIR}/compile_this.cpp)

target_link_libraries(addons PUBLIC fmt::fmt)
target_compile_options(zenith PRIVATE -Wreorder -Wconversion -Wformat-security -Wshadow)
target_link_libraries(zenith PRIVATE addons nativehelper fmt::fmt-header-only)
