cmake_minimum_required(VERSION 3.18)

# FORCE_COMPILER macros have to be used, because arm-none-eabi compilers don't pass the cmake simple program compilation test
include (CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc arm-none-eabi-gcc)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ arm-none-eabi-g++)

set(gnu_arm_embedded_toolchain_version 10.3-2021.10)
set(gnu_arm_embedded_toolchain_stdlib_version 10.3.1)

add_library(StdLib INTERFACE)
target_include_directories(StdLib INTERFACE
    /usr/share/gcc-arm-none-eabi-${gnu_arm_embedded_toolchain_version}/arm-none-eabi/include/c++/${gnu_arm_embedded_toolchain_stdlib_version}
    /usr/share/gcc-arm-none-eabi-${gnu_arm_embedded_toolchain_version}/arm-none-eabi/include/c++/${gnu_arm_embedded_toolchain_stdlib_version}/arm-none-eabi/
)

# set the project name
project(Firmware)

set(arch_flags
    -mthumb  # 32-bit address space
    -mcpu=cortex-m4
    -mfloat-abi=hard  # use hardware floating point unit
    -mfpu=fpv4-sp-d16  # select a floating point unit (there is only one)
    -fshort-enums
    -specs=nano.specs
    -specs=nosys.specs
    #--sysroot=/usr/share/gcc-arm-none-eabi-${gnu_arm_embedded_toolchain_version}/arm-none-eabi
)

set(c_flags
    ${arch_flags}
    #--target=arm-none-eabi
    -O0  #optimization level
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
    -std=c++11
)
string(REPLACE ";" " " cxx_flags_string "${cxx_flags}")
set(CMAKE_CXX_FLAGS "${cxx_flags_string}")

# add the executable
add_executable(Firmware src/main.cpp)

target_link_libraries(Firmware PRIVATE Src)
target_link_options(Firmware PRIVATE
    # -fuse-ld=/usr/bin/arm-none-eabi-ld
    -Wl,--gc-sections
    # -Wl,--warn-once
    -fdiagnostics-color=always
    # -specs=nano.specs
    # -specs=nosys.specs
)

add_subdirectory(src)
add_subdirectory(submodules)
add_subdirectory(middlewares)
