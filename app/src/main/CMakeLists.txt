
cmake_minimum_required(VERSION 3.22.1)

project(ZenithEmu LANGUAGES CXX ASM VERSION 1.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Wall -march=armv8-a+crc -fvisibility=hidden")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast -fno-stack-protector -fomit-frame-pointer -flto=full")
set(CMAKE_CXX_FLAGS_DEBUG "-glldb -fstack-protector-strong -O0")

set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-z,noexecstack")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "-s -Wl,--gc-sections")

# zenith: PS2 core emulator native library
add_library(zenith MODULE)

set(ZENITH_DIR ${CMAKE_SOURCE_DIR}/cpp/zenith)
set(ADDONS_DIR ${CMAKE_SOURCE_DIR}/cpp/addons)
set(ZENITH_MISC_DIR ${CMAKE_SOURCE_DIR}/cpp)

target_sources(zenith PRIVATE
    ${ZENITH_DIR}/common/app.cpp
    ${ZENITH_DIR}/common/logger.cpp
    ${ZENITH_DIR}/cpu/cyclic32.cpp
    ${ZENITH_DIR}/cpu/verify_features.cpp
    ${ZENITH_DIR}/eeiv/ee_engine.cpp
    ${ZENITH_DIR}/eeiv/ee_iov0.cpp
    ${ZENITH_DIR}/eeiv/c0/cop_cache.cpp
    ${ZENITH_DIR}/eeiv/c0/cop0.cpp
    ${ZENITH_DIR}/eeiv/mmu_tlb.cpp
    ${ZENITH_DIR}/eeiv/fuji/mipsiv_full_interpreter.cpp
    ${ZENITH_DIR}/gs/synth_tables.cpp
    ${ZENITH_DIR}/hle/bios_patch.cpp
    ${ZENITH_DIR}/hle/group_mgr.cpp
    ${ZENITH_DIR}/hle/bios_class.cpp
    ${ZENITH_DIR}/fs/bios_loader.cpp
    ${ZENITH_DIR}/iop/iop_core.cpp
    ${ZENITH_DIR}/gpu/hw_render.cpp
    ${ZENITH_DIR}/gpu/render_driver.cpp
    ${ZENITH_DIR}/gpu/exhibition_engine.cpp
    ${ZENITH_DIR}/os/system_state.cpp
    ${ZENITH_DIR}/java/device_handler.cpp
    ${ZENITH_DIR}/java/jclasses.cpp
    ${ZENITH_DIR}/console/virt_devices.cpp
    ${ZENITH_DIR}/console/emu_vm.cpp
    ${ZENITH_DIR}/console/emu_thread.cpp
    ${ZENITH_DIR}/console/sched_logical.cpp
    ${ZENITH_MISC_DIR}/jvm_comm.cpp
    ${ZENITH_MISC_DIR}/drivers_glvk_jni.cpp
    ${ZENITH_MISC_DIR}/bios_jni.cpp

    ${ADDONS_DIR}/compile_this.cpp)

add_subdirectory("${ADDONS_DIR}/fmt" EXCLUDE_FROM_ALL)
add_subdirectory("${ADDONS_DIR}/range-v3")

# Using specific usable and secure Android structures
add_compile_definitions(VK_USE_PLATFORM_ANDROID_KHR)
add_compile_definitions(VULKAN_HPP_NO_SMART_HANDLE)
# We need to disable static loaders to enable the implementation of custom drivers
add_compile_definitions(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
add_compile_definitions(VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL=0)

include_directories(SYSTEM "${ADDONS_DIR}/Vulkan-Hpp")
include_directories(SYSTEM "${ADDONS_DIR}/Vulkan-Hpp/Vulkan-Headers/include")

# Define a static library for Perfetto
add_library(perfetto STATIC ${ADDONS_DIR}/perfetto/sdk/perfetto.cc)
target_include_directories(zenith PRIVATE ${ZENITH_MISC_DIR} ${ZENITH_DIR}
    ${ADDONS_DIR}/perfetto/sdk)

target_link_libraries(zenith PRIVATE log android nativehelper nativewindow EGL fmt::fmt perfetto range-v3::range-v3)
target_compile_options(zenith PRIVATE -Wreorder -Wconversion -Wformat-security -Wshadow)
