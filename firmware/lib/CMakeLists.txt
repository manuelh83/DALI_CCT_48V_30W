add_library(stm32cubeg0 INTERFACE)

set(STM32CUBEG0_DIR ${CMAKE_CURRENT_LIST_DIR}/STM32CubeG0)
if(EXISTS ${STM32CUBEG0_DIR})
    target_include_directories(stm32cubeg0 INTERFACE
        ${STM32CUBEG0_DIR}/Drivers/CMSIS/Include
        ${STM32CUBEG0_DIR}/Drivers/CMSIS/Device/ST/STM32G0xx/Include
        ${STM32CUBEG0_DIR}/Drivers/STM32G0xx_HAL_Driver/Inc
    )
endif()
