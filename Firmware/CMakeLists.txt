cmake_minimum_required(VERSION 3.18)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)
set(DFU_TOOL ${CMAKE_CURRENT_LIST_DIR}/submodules/dfuse-tool/dfu-convert)

set(CMAKE_CXX_STANDARD 11)

project(midi-grid CXX C ASM)

set(arch_flags
    -mthumb  # 32-bit address space
    -mcpu=cortex-m4
    -mfloat-abi=hard  # use hardware floating point unit
    -mfpu=fpv4-sp-d16  # select a floating point unit (there is only one)
    -mlittle-endian
    -fshort-enums
    -specs=nano.specs
    -specs=nosys.specs
)

set(c_flags
    ${arch_flags}
    -O1  # optimization level
    -ggdb3
    -ffunction-sections
    -fdata-sections
    -fdiagnostics-color=always  # enable color output of compiler
)
string(REPLACE ";" " " c_flags_string "${c_flags}")
set(CMAKE_C_FLAGS "${c_flags_string}")

set(cxx_flags
    ${c_flags}
    -fno-rtti
    -fno-strict-aliasing
)
string(REPLACE ";" " " cxx_flags_string "${cxx_flags}")
set(CMAKE_CXX_FLAGS "${cxx_flags_string}")

# add the executable
add_executable(${PROJECT_NAME}.elf
    src/main.cpp
    submodules/cmsis_device_f4/Source/Templates/gcc/startup_stm32f413xx.s
)

set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F413RHTx_FLASH.ld -Wl,-Map=${PROJECT_NAME}.map")

target_link_libraries(${PROJECT_NAME}.elf PRIVATE Src)
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -Wl,--gc-sections
    -fdiagnostics-color=always
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}.elf)
add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${PROJECT_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex)
add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)
add_custom_target(${PROJECT_NAME}.dfu ALL DEPENDS ${PROJECT_NAME}.hex COMMAND ${DFU_TOOL} -i ${PROJECT_NAME}.hex ${PROJECT_NAME}.dfu)

add_subdirectory(src)
add_subdirectory(submodules)
add_subdirectory(middlewares)
