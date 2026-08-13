cmake_minimum_required(VERSION 3.20)
project(dali_cct_firmware C ASM)

enable_testing()

option(FIRMWARE_BUILD_TESTS "Build host-side unit tests" ON)
option(FIRMWARE_BUILD_EMBEDDED "Build ARM firmware image" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

set(FIRMWARE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(FIRMWARE_SRC_DIR ${FIRMWARE_ROOT}/src)

set(FIRMWARE_COMMON_SOURCES
    ${FIRMWARE_SRC_DIR}/hal/hal_gpio.c
    ${FIRMWARE_SRC_DIR}/hal/hal_adc.c
    ${FIRMWARE_SRC_DIR}/hal/hal_i2c.c
    ${FIRMWARE_SRC_DIR}/hal/hal_dac_i2c.c
    ${FIRMWARE_SRC_DIR}/hal/hal_eeprom_i2c.c
    ${FIRMWARE_SRC_DIR}/hal/hal_usart.c
    ${FIRMWARE_SRC_DIR}/hal/hal_timer.c
    ${FIRMWARE_SRC_DIR}/dali/dali_physical.c
    ${FIRMWARE_SRC_DIR}/dali/dali_link.c
    ${FIRMWARE_SRC_DIR}/dali/dali_application.c
    ${FIRMWARE_SRC_DIR}/dali/dali_device_type.c
    ${FIRMWARE_SRC_DIR}/control/current_sink.c
    ${FIRMWARE_SRC_DIR}/control/cct_mixing.c
    ${FIRMWARE_SRC_DIR}/control/fade_engine.c
    ${FIRMWARE_SRC_DIR}/control/power_limiter.c
    ${FIRMWARE_SRC_DIR}/nvm/eeprom_driver.c
    ${FIRMWARE_SRC_DIR}/nvm/nvm_handler.c
    ${FIRMWARE_SRC_DIR}/nvm/wear_leveling.c
    ${FIRMWARE_SRC_DIR}/fault/protection.c
    ${FIRMWARE_SRC_DIR}/fault/fault_log.c
    ${FIRMWARE_SRC_DIR}/fault/ntc_thermistor.c
    ${FIRMWARE_SRC_DIR}/system/scheduler.c
    ${FIRMWARE_SRC_DIR}/system/board_init.c
    ${FIRMWARE_SRC_DIR}/system/debug.c
)

add_library(firmware_core STATIC ${FIRMWARE_COMMON_SOURCES})
target_include_directories(firmware_core
    PUBLIC
        ${FIRMWARE_SRC_DIR}
        ${FIRMWARE_SRC_DIR}/hal
        ${FIRMWARE_SRC_DIR}/dali
        ${FIRMWARE_SRC_DIR}/control
        ${FIRMWARE_SRC_DIR}/nvm
        ${FIRMWARE_SRC_DIR}/fault
        ${FIRMWARE_SRC_DIR}/system
)
target_compile_definitions(firmware_core PUBLIC HOST_BUILD=1)
target_compile_options(firmware_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(firmware_core PUBLIC m)

if(NOT FIRMWARE_BUILD_EMBEDDED)
    add_executable(firmware_host ${FIRMWARE_SRC_DIR}/main.c)
    target_link_libraries(firmware_host PRIVATE firmware_core)
    target_compile_definitions(firmware_host PRIVATE HOST_BUILD=1)
    target_compile_options(firmware_host PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(lib)

if(FIRMWARE_BUILD_TESTS)
    add_subdirectory(test)
endif()

if(FIRMWARE_BUILD_EMBEDDED)
    add_executable(firmware.elf
        ${FIRMWARE_COMMON_SOURCES}
        ${FIRMWARE_SRC_DIR}/main.c
        ${FIRMWARE_SRC_DIR}/startup_stm32g031k8tx.s
        ${FIRMWARE_SRC_DIR}/stm32g031xx_it.c
        ${FIRMWARE_SRC_DIR}/system_stm32g031xx.c
    )
    target_include_directories(firmware.elf
        PRIVATE
            ${FIRMWARE_SRC_DIR}
            ${FIRMWARE_SRC_DIR}/hal
            ${FIRMWARE_SRC_DIR}/dali
            ${FIRMWARE_SRC_DIR}/control
            ${FIRMWARE_SRC_DIR}/nvm
            ${FIRMWARE_SRC_DIR}/fault
            ${FIRMWARE_SRC_DIR}/system
    )
    target_compile_options(firmware.elf PRIVATE -Wall -Wextra -Wpedantic -Werror -ffreestanding -fdata-sections -ffunction-sections)
    target_link_options(firmware.elf PRIVATE
        -T${FIRMWARE_ROOT}/stm32g031k8tx_flash.ld
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_BINARY_DIR}/firmware.map
        -nostartfiles
    )
    target_link_libraries(firmware.elf PRIVATE stm32cubeg0 m)

    find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
    if(CMAKE_OBJCOPY)
        add_custom_command(TARGET firmware.elf POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:firmware.elf> ${CMAKE_BINARY_DIR}/firmware.hex
            BYPRODUCTS ${CMAKE_BINARY_DIR}/firmware.hex
            COMMENT "Generating firmware.hex"
        )
    endif()
endif()
