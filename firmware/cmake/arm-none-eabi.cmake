# ARM Cortex-M0+ Toolchain File for CMake
# STM32G031K8T6 (arm-none-eabi-gcc)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

# Compiler settings
set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER arm-none-eabi-g++ CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc CACHE FILEPATH "ASM compiler")
set(CMAKE_AR arm-none-eabi-ar CACHE FILEPATH "Archiver")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE FILEPATH "Object copy utility")
set(CMAKE_OBJDUMP arm-none-eabi-objdump CACHE FILEPATH "Object dump utility")
set(CMAKE_SIZE arm-none-eabi-size CACHE FILEPATH "Size utility")

# Compiler flags for ARM Cortex-M0+
set(CMAKE_C_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Wall -Wextra -Werror -ffunction-sections -fdata-sections" CACHE STRING "C compiler flags")
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m0plus -mthumb -x assembler-with-cpp" CACHE STRING "ASM compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m0plus -mthumb -Wl,--gc-sections -Wl,--print-memory-usage" CACHE STRING "Linker flags")

# Disable compiler checks for cross-compilation
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Set build type defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optimization for embedded
if(CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "Release C flags")
else()
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g3" CACHE STRING "Debug C flags")
endif()
