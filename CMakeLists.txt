cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

# Select the target architecture (x86_64 | riscv64)
if(NOT DEFINED TOPIOS_ARCH)
    set(TOPIOS_ARCH x86_64)
endif()

# Enable the proper assembly language provider per architecture.
# NASM builds the x86 boot stub, GNU as builds the RISC-V boot stub.
if(TOPIOS_ARCH STREQUAL "riscv64")
    set(TOPIOS_LANGUAGES C CXX ASM)
else()
    set(TOPIOS_LANGUAGES C CXX ASM_NASM)
endif()

project(TopiOS
    VERSION 0.1.0
    LANGUAGES ${TOPIOS_LANGUAGES}
)

if(TOPIOS_ARCH STREQUAL "riscv64")
    set(TOPIOS_BOOT_SRC boot/riscv64/boot.S)
    set(TOPIOS_LINKER ${CMAKE_SOURCE_DIR}/linker-riscv64.ld)
    set(TOPIOS_KERNEL_SRC
        kernel/riscv/kernel_riscv.cpp
        kernel/riscv/uart.cpp
        kernel/riscv/virtio.cpp
    )
else()
    set(TOPIOS_BOOT_SRC boot/x86_64/boot.asm)
    set(TOPIOS_LINKER ${CMAKE_SOURCE_DIR}/linker.ld)
    set(TOPIOS_KERNEL_SRC
        kernel/kernel.cpp
        kernel/vga.cpp
        kernel/serial.cpp
        kernel/keyboard.cpp
    )
endif()

add_executable(kernel.elf
    ${TOPIOS_KERNEL_SRC}
    ${TOPIOS_BOOT_SRC}
)

target_compile_options(kernel.elf PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector>
    $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
)

target_link_options(kernel.elf PRIVATE
    -nostdlib
    -static
    -T${TOPIOS_LINKER}
)