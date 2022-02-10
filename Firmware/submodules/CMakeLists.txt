
# CMSIS core
add_library(CmsisCore INTERFACE)
target_include_directories(CmsisCore INTERFACE ./cmsis_core/Include)

# CMSIS device
add_library(CmsisDevice INTERFACE)
target_include_directories(CmsisDevice INTERFACE ./cmsis_device_f4/Include)

# ST HAL
set(st_hal_dir ./stm32f4xx_hal_driver)
set(st_hal_source_dir ${st_hal_dir}/Src)

add_library(StHal STATIC
    ${st_hal_source_dir}/stm32f4xx_hal.c
    ${st_hal_source_dir}/stm32f4xx_hal_adc.c
    ${st_hal_source_dir}/stm32f4xx_hal_adc_ex.c
    ${st_hal_source_dir}/stm32f4xx_hal_cortex.c
    ${st_hal_source_dir}/stm32f4xx_hal_dma.c
    ${st_hal_source_dir}/stm32f4xx_hal_dma_ex.c
    ${st_hal_source_dir}/stm32f4xx_hal_flash.c
    ${st_hal_source_dir}/stm32f4xx_hal_flash_ex.c
    ${st_hal_source_dir}/stm32f4xx_hal_flash_ramfunc.c
    ${st_hal_source_dir}/stm32f4xx_hal_gpio.c
    ${st_hal_source_dir}/stm32f4xx_hal_pcd.c
    ${st_hal_source_dir}/stm32f4xx_hal_pcd_ex.c
    ${st_hal_source_dir}/stm32f4xx_hal_pwr.c
    ${st_hal_source_dir}/stm32f4xx_hal_pwr_ex.c
    ${st_hal_source_dir}/stm32f4xx_hal_rcc.c
    ${st_hal_source_dir}/stm32f4xx_hal_rcc_ex.c
    ${st_hal_source_dir}/stm32f4xx_hal_spi.c
    ${st_hal_source_dir}/stm32f4xx_hal_tim.c
    ${st_hal_source_dir}/stm32f4xx_hal_tim_ex.c
    ${st_hal_source_dir}/stm32f4xx_hal_uart.c
    ${st_hal_source_dir}/stm32f4xx_ll_usb.c
)

target_include_directories(StHal PUBLIC ${st_hal_dir}/Inc)
target_link_libraries(StHal PUBLIC
    StdLib
    Configs
    CmsisCore
    CmsisDevice
)

target_compile_definitions(StHal PUBLIC STM32F411xE) # USE_HAL_DRIVER)

# FreeRTOS kernel
set(freertos_kernel_dir ./FreeRTOS-Kernel)

add_library(FreertosKernel STATIC
    ${freertos_kernel_dir}/croutine.c
    ${freertos_kernel_dir}/event_groups.c
    ${freertos_kernel_dir}/list.c
    ${freertos_kernel_dir}/queue.c
    ${freertos_kernel_dir}/stream_buffer.c
    ${freertos_kernel_dir}/tasks.c
    ${freertos_kernel_dir}/timers.c
    ${freertos_kernel_dir}/portable/GCC/ARM_CM4F/port.c
    ${freertos_kernel_dir}/portable/MemMang/heap_1.c
)

target_include_directories(FreertosKernel PUBLIC
    ${freertos_kernel_dir}/include
    ${freertos_kernel_dir}/portable/GCC/ARM_CM4F
)
target_link_libraries(FreertosKernel PUBLIC Configs)

# FreeRTOS wrappers
set(freertos_wrappers_dir ./freertos-addons/c++/Source)

add_library(FreertosWrappers STATIC
    ${freertos_wrappers_dir}/cqueue.cpp
    ${freertos_wrappers_dir}/csemaphore.cpp
    ${freertos_wrappers_dir}/cthread.cpp
    ${freertos_wrappers_dir}/cmutex.cpp
)

target_include_directories(FreertosWrappers PUBLIC ${freertos_wrappers_dir}/include)
target_link_libraries(FreertosWrappers PUBLIC
    StdLib
    FreertosKernel
)
target_compile_definitions(FreertosWrappers PRIVATE
    CPP_FREERTOS_NO_EXCEPTIONS
    CPP_FREERTOS_NO_CPP_STRINGS
)

# ETL
add_library(Etl INTERFACE)
target_include_directories(Etl INTERFACE ./etl/include)
target_link_libraries(Etl INTERFACE Configs)
