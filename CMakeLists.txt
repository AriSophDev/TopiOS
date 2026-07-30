cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(TopiOS
    VERSION 0.1.0
    LANGUAGES C CXX ASM
)


add_executable(kernel.elf kernel/kernel.cpp kernel/vga.cpp)

target_compile_options(kernel.elf PRIVATE
    -ffreestanding
    -fno-exceptions
    -fno-rtti
)

target_link_options(kernel.elf PRIVATE
    -nostdlib
    -T${CMAKE_SOURCE_DIR}/linker.ld
)
