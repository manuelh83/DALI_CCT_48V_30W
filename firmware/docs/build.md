# Build Guide

## Prerequisites

- CMake 3.20+
- Native GCC/Clang for host tests
- `arm-none-eabi-gcc` and `arm-none-eabi-objcopy` for embedded output
- Optional STM32CubeG0 submodule under `firmware/lib/STM32CubeG0`

## Host Build and Tests

```bash
cmake -S firmware -B firmware/build -DFIRMWARE_BUILD_TESTS=ON
cmake --build firmware/build
ctest --test-dir firmware/build --output-on-failure
./firmware/build/firmware_host
```

## Embedded Build

```bash
git submodule add https://github.com/STMicroelectronics/STM32CubeG0.git firmware/lib/STM32CubeG0
cmake -S firmware -B firmware/build-embedded \
  -DFIRMWARE_BUILD_TESTS=OFF \
  -DFIRMWARE_BUILD_EMBEDDED=ON \
  -DCMAKE_TOOLCHAIN_FILE=firmware/cmake/arm-none-eabi.cmake
cmake --build firmware/build-embedded
```

The embedded build emits `firmware/build-embedded/firmware.hex` when `arm-none-eabi-objcopy` is available.

## Flashing

Use STM32CubeProgrammer or STM32CubeIDE to flash `firmware.hex` over SWD.
