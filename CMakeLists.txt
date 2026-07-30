cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

project(TopiOS
    VERSION 0.1.0
    LANGUAGES C CXX ASM_NASM
)


add_executable(kernel.elf 
    kernel/kernel.cpp 
    kernel/vga.cpp 
    kernel/serial.cpp 
    kernel/keyboard.cpp 
    boot/boot.asm
)

target_compile_options(kernel.elf PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

target_link_options(kernel.elf PRIVATE
    -nostdlib
    -T${CMAKE_SOURCE_DIR}/linker.ld
)
