cmake_minimum_required(VERSION 3.12.1)

include (CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("arm-none-eabi-gcc" GNU)
CMAKE_FORCE_CXX_COMPILER ("arm-none-eabi-g++" GNU)

# set the project name
project(Project)

# add the executable
add_executable(Project
    Src/application/Application.cpp
    Src/application/grid_test/GridTest.cpp
    Src/application/internal_menu/InternalMenu.cpp
    Src/application/launchpad/Launchpad.cpp
    Src/application/launchpad/LcdGui.cpp
    Src/application/snake/Snake.cpp
    Src/application/startup/Startup.cpp
    Src/hardware/grid/GridDriver.cpp
    Src/io/additional_buttons/AdditionalButtons.cpp
    Src/io/grid/ButtonInput.cpp
    Src/io/grid/FlashingLeds.cpp
    Src/io/grid/Grid.cpp
    Src/io/grid/LedOutput.cpp
    Src/io/grid/PulsingLeds.cpp
    Src/io/lcd/Backlight.cpp
    Src/io/lcd/Lcd.cpp
    Src/io/lcd/LcdDriver.cpp
    Src/io/rotary_controls/RotaryControls.cpp
    Src/io/usb/UsbMidi.cpp
    Src/main.cpp
    Src/system/GlobalInterrupts.cpp
    Src/system/System.cpp
    Src/types/Color.cpp
    submodules/freertos-addons/c++/Source/cqueue.cpp
    submodules/freertos-addons/c++/Source/csemaphore.cpp
    submodules/freertos-addons/c++/Source/cthread.cpp
    submodules/freertos-addons/c++/Source/cmutex.cpp

    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
    Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    Middlewares/USBMIDI/Src/usbd_midi.c
    Src/configs/heap_2.c
    Src/configs/port.c
    Src/io/usb/usbd_conf.c
    Src/io/usb/usbd_desc.c
    Src/io/usb/usb_device.c
    Src/system/stm32f4xx_it.c
    Src/system/system_stm32f4xx.c
    submodules/FreeRTOS-Kernel/croutine.c
    submodules/FreeRTOS-Kernel/event_groups.c 
    submodules/FreeRTOS-Kernel/list.c
    submodules/FreeRTOS-Kernel/queue.c
    submodules/FreeRTOS-Kernel/stream_buffer.c
    submodules/FreeRTOS-Kernel/tasks.c
    submodules/FreeRTOS-Kernel/timers.c

    startup/startup_stm32f411xe.s
)

target_include_directories(Project
    PRIVATE Drivers/CMSIS/Include
    PRIVATE Drivers/CMSIS/Device/ST/STM32F4xx/Include
    PRIVATE Drivers/STM32F4xx_HAL_Driver/Inc
    PRIVATE Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    PRIVATE Middlewares/USBMIDI/Inc
    PRIVATE Src
    PRIVATE Src/configs
    PRIVATE submodules/etl/include
    PRIVATE submodules/FreeRTOS-Kernel/include
    PRIVATE submodules/freertos-addons/c++/Source/include
    PRIVATE Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
)

target_compile_options(Project
    PRIVATE -mcpu=cortex-m4
    PRIVATE -DUSE_HAL_DRIVER
    PRIVATE -DSTM32F411xE
    PRIVATE -mthumb
    PRIVATE -specs=nano.specs
    PRIVATE -specs=nosys.specs
    PRIVATE -mfloat-abi=hard
    PRIVATE -mfpu=fpv4-sp-d16

    PRIVATE -O0 
    PRIVATE -ggdb3
    PRIVATE -ffunction-sections
    PRIVATE -fdata-sections

    PRIVATE -fno-rtti
    PRIVATE -fno-exceptions
    PRIVATE -fno-strict-aliasing
    PRIVATE -std=c++11
    PRIVATE -DCPP_FREERTOS_NO_EXCEPTIONS
    PRIVATE -DCPP_FREERTOS_NO_CPP_STRINGS
)

set_target_properties(Project PROPERTIES LINK_DEPENDS STM32F411RCTx_FLASH.ld)

target_link_libraries(Project
    PRIVATE -mcpu=cortex-m4
    PRIVATE -DUSE_HAL_DRIVER
    PRIVATE -DSTM32F411xE
    PRIVATE -mthumb
    PRIVATE -specs=nano.specs
    PRIVATE -specs=nosys.specs
    PRIVATE -mfloat-abi=hard
    PRIVATE -mfpu=fpv4-sp-d16 )
