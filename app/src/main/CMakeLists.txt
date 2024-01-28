
cmake_minimum_required(VERSION 3.22.1)

project(CosmicEmu LANGUAGES CXX VERSION 0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-conversion -march=armv8-a+crc -fvisibility=hidden")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast -fno-stack-protector -fomit-frame-pointer -flto=full")
set(CMAKE_CXX_FLAGS_DEBUG "-glldb -fstack-protector-strong -O0")

set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-z,noexecstack")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "-s -Wl,--gc-sections")

# cosmic: PS2 core emulator native library
add_library(cosmic MODULE)

set(COSMIC_DIR ${CMAKE_SOURCE_DIR}/cpp/cosmic)
set(ADDONS_DIR ${CMAKE_SOURCE_DIR}/cpp/addons)
set(COSMIC_MISC_DIR ${CMAKE_SOURCE_DIR}/cpp)
target_sources(cosmic PRIVATE
    ${COSMIC_DIR}/common/app.cpp
    ${COSMIC_DIR}/common/logger.cpp
    ${COSMIC_DIR}/common/except.cpp
    ${COSMIC_DIR}/cpu/cyclic32.cpp
    ${COSMIC_DIR}/cpu/verify_features.cpp
    ${COSMIC_DIR}/engine/ee_core.cpp
    ${COSMIC_DIR}/engine/ee_plus.cpp
    ${COSMIC_DIR}/engine/ee_timers.cpp
    ${COSMIC_DIR}/engine/ee_intc.cpp
    ${COSMIC_DIR}/engine/copctrl/cop_cache.cpp
    ${COSMIC_DIR}/engine/copctrl/cop0.cpp
    ${COSMIC_DIR}/engine/copctrl/cop_misc.cpp
    ${COSMIC_DIR}/engine/copctrl/cop_dma.cpp
    ${COSMIC_DIR}/engine/cop1_fu.cpp
    ${COSMIC_DIR}/mio/mmu_tlb.cpp
    ${COSMIC_DIR}/mio/tlb_mapper.cpp
    ${COSMIC_DIR}/mio/dma_ctrl.cpp
    ${COSMIC_DIR}/mio/dma_move.cpp
    ${COSMIC_DIR}/mio/mem_pipe.cpp
    ${COSMIC_DIR}/mio/blocks.cpp
    ${COSMIC_DIR}/creeper/inst_operands.cpp
    ${COSMIC_DIR}/creeper/fastmem.cpp
    ${COSMIC_DIR}/creeper/psx/iop_interpreter.cpp
    ${COSMIC_DIR}/creeper/psx/psx_ep1.cpp
    ${COSMIC_DIR}/creeper/ee/fpu.cpp
    ${COSMIC_DIR}/creeper/ee/mipsiv_cached.cpp
    ${COSMIC_DIR}/creeper/ee/mipsiv_opcodes.cpp
    ${COSMIC_DIR}/creeper/ee/mipsiv_isa.cpp
    ${COSMIC_DIR}/creeper/ee/mipsiv_math.cpp
    ${COSMIC_DIR}/creeper/ee/cop_isa.cpp
    ${COSMIC_DIR}/creeper/micro/vum_code.cpp
    ${COSMIC_DIR}/creeper/micro/int_lower1.cpp
    ${COSMIC_DIR}/creeper/micro/float_vectorization.cpp
    ${COSMIC_DIR}/gs/synth_tables.cpp
    ${COSMIC_DIR}/gs/synth_engine.cpp
    ${COSMIC_DIR}/gs/gif_bridge.cpp
    ${COSMIC_DIR}/gs/transfer_queue.cpp
    ${COSMIC_DIR}/gs/gif_packed.cpp
    ${COSMIC_DIR}/vu/vecu.cpp
    ${COSMIC_DIR}/vu/vu_time.cpp
    ${COSMIC_DIR}/vu/vif10_upload.cpp
    ${COSMIC_DIR}/vu/v01_cop2vu.cpp
    ${COSMIC_DIR}/vu/vif_fifo.cpp
    ${COSMIC_DIR}/vu/vu1_xgkick.cpp
    ${COSMIC_DIR}/hle/bios_patch.cpp
    ${COSMIC_DIR}/hle/group_mgr.cpp
    ${COSMIC_DIR}/hle/bios_info.cpp
    ${COSMIC_DIR}/hle/syscall_gate.cpp
    ${COSMIC_DIR}/fs/bios_loader.cpp
    ${COSMIC_DIR}/iop/iop_core.cpp
    ${COSMIC_DIR}/iop/iop_cop.cpp
    ${COSMIC_DIR}/iop/iop_intc.cpp
    ${COSMIC_DIR}/iop/iop_dma.cpp
    ${COSMIC_DIR}/gpu/render_driver.cpp
    ${COSMIC_DIR}/gpu/exhibition_engine.cpp
    ${COSMIC_DIR}/gpu/graphics_layer.cpp
    ${COSMIC_DIR}/gpu/vulcano/vulkan_context.cpp
    ${COSMIC_DIR}/gpu/vulcano/vulkan_misc.cpp
    ${COSMIC_DIR}/gpu/vulcano/vram_allocator.cpp
    ${COSMIC_DIR}/os/system_state.cpp
    ${COSMIC_DIR}/java/device_handler.cpp
    ${COSMIC_DIR}/java/jclasses.cpp
    ${COSMIC_DIR}/console/virt_devices.cpp
    ${COSMIC_DIR}/vm/emu_vm.cpp
    ${COSMIC_DIR}/vm/emu_thread.cpp
    ${COSMIC_DIR}/vm/sched_logical.cpp
    ${COSMIC_DIR}/vm/step_frameloop.cpp
    ${COSMIC_DIR}/vm/watch.cpp
    ${COSMIC_DIR}/console/intc.cpp
    ${COSMIC_DIR}/console/backdoor.cpp
    ${COSMIC_DIR}/fishron/emitter_common.cpp
    ${COSMIC_DIR}/fishron/ee64/jitter_arm64_ee.cpp
    ${COSMIC_DIR}/ipu/chrome_table.cpp
    ${COSMIC_DIR}/ipu/ipu_core.cpp
    ${COSMIC_DIR}/ipu/decoder_fifo.cpp
    ${COSMIC_DIR}/pshook/hk_psx.cpp
    ${COSMIC_DIR}/pshook/psx_native.cpp
    ${COSMIC_DIR}/gamedb/title_patches.cpp
    ${COSMIC_DIR}/cdvd/cd_header.cpp
    ${COSMIC_DIR}/input/memcard.cpp
    ${COSMIC_DIR}/input/padjoy.cpp
    ${COSMIC_DIR}/input/sio2_io.cpp
    ${COSMIC_DIR}/snd/stream_buffer.cpp
    ${COSMIC_DIR}/spu/sound_core.cpp
    ${COSMIC_DIR}/usb/dev_ir.cpp
    ${COSMIC_MISC_DIR}/jvm_comm.cpp
    ${COSMIC_MISC_DIR}/drivers_glvk_jni.cpp
    ${COSMIC_MISC_DIR}/bios_jni.cpp
    ${COSMIC_MISC_DIR}/emu_user.cpp
    ${ADDONS_DIR}/compile_this.cpp)
add_subdirectory("${ADDONS_DIR}/fmt" EXCLUDE_FROM_ALL)
add_subdirectory("${ADDONS_DIR}/range-v3")

# Using specific usable and secure Android structures
add_compile_definitions(VK_USE_PLATFORM_ANDROID_KHR)
add_compile_definitions(VULKAN_HPP_NO_SMART_HANDLE)
add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)
# We need to disable static loaders to enable the implementation of custom drivers
add_compile_definitions(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
add_compile_definitions(VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL=0)

include_directories(SYSTEM "${ADDONS_DIR}/Vulkan-Hpp")
include_directories(SYSTEM "${ADDONS_DIR}/Vulkan-Hpp/Vulkan-Headers/include")
include_directories("${ADDONS_DIR}/VulkanMemoryAllocator/include")

# Define a static library for Perfetto
add_library(perfetto STATIC ${ADDONS_DIR}/perfetto/sdk/perfetto.cc)
target_include_directories(cosmic PRIVATE ${COSMIC_MISC_DIR} ${COSMIC_DIR} ${ADDONS_DIR}/perfetto/sdk)

target_link_libraries(cosmic PRIVATE log android nativehelper nativewindow EGL fmt::fmt perfetto range-v3::range-v3)
target_compile_options(cosmic PRIVATE -Wreorder -Wconversion -Wformat-security -Wshadow)
