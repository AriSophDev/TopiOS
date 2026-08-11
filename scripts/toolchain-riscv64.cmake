set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

set(CMAKE_C_COMPILER riscv64-elf-gcc)
set(CMAKE_CXX_COMPILER riscv64-elf-g++)
set(CMAKE_ASM_COMPILER riscv64-elf-gcc)

# RISC-V 64-bit, general-purpose ISA + atomic
set(TOPIOS_MARCH rv64gc)
set(TOPIOS_MABI lp64)

# medany: the kernel is linked at 0x80200000 (> 2 GiB), outside the range of
# the default "medlow" code model which requires symbols within +/- 2 GiB.
set(TOPIOS_MCODE -mcmodel=medany)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=${TOPIOS_MARCH} -mabi=${TOPIOS_MABI} ${TOPIOS_MCODE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${TOPIOS_MARCH} -mabi=${TOPIOS_MABI} ${TOPIOS_MCODE}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=${TOPIOS_MARCH} -mabi=${TOPIOS_MABI}")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
