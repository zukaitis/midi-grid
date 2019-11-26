# ELF target name
TARGET=output/testmode09.elf

# C source files
SRC=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
SRC+=Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
SRC+=Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
SRC+=Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
SRC+=Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
SRC+=Middlewares/USBMIDI/Src/usbd_midi.c
SRC+=Src/configs/heap_2.c
SRC+=Src/configs/port.c
SRC+=Src/io/usb/usbd_conf.c
SRC+=Src/io/usb/usbd_desc.c
SRC+=Src/io/usb/usb_device.c
SRC+=Src/system/stm32f4xx_it.c
SRC+=Src/system/system_stm32f4xx.c
SRC+=submodules/FreeRTOS-Kernel/croutine.c
SRC+=submodules/FreeRTOS-Kernel/event_groups.c 
SRC+=submodules/FreeRTOS-Kernel/list.c
SRC+=submodules/FreeRTOS-Kernel/queue.c
SRC+=submodules/FreeRTOS-Kernel/stream_buffer.c
SRC+=submodules/FreeRTOS-Kernel/tasks.c
SRC+=submodules/FreeRTOS-Kernel/timers.c

# C++ source files
CXXSRC=Src/application/Application.cpp
CXXSRC+=Src/application/grid_test/GridTest.cpp
CXXSRC+=Src/application/internal_menu/InternalMenu.cpp
CXXSRC+=Src/application/launchpad/Launchpad.cpp
CXXSRC+=Src/application/launchpad/LcdGui.cpp
CXXSRC+=Src/application/snake/Snake.cpp
CXXSRC+=Src/application/startup/Startup.cpp
CXXSRC+=Src/hardware/grid/GridDriver.cpp
CXXSRC+=Src/io/additional_buttons/AdditionalButtons.cpp
CXXSRC+=Src/io/grid/ButtonInput.cpp
CXXSRC+=Src/io/grid/FlashingLeds.cpp
CXXSRC+=Src/io/grid/Grid.cpp
CXXSRC+=Src/io/grid/LedOutput.cpp
CXXSRC+=Src/io/grid/PulsingLeds.cpp
CXXSRC+=Src/io/lcd/Backlight.cpp
CXXSRC+=Src/io/lcd/Lcd.cpp
CXXSRC+=Src/io/lcd/LcdDriver.cpp
CXXSRC+=Src/io/rotary_controls/RotaryControls.cpp
CXXSRC+=Src/io/usb/UsbMidi.cpp
CXXSRC+=Src/main.cpp
CXXSRC+=Src/system/GlobalInterrupts.cpp
CXXSRC+=Src/system/System.cpp
CXXSRC+=Src/types/Color.cpp
CXXSRC+=submodules/freertos-addons/c++/Source/cqueue.cpp
CXXSRC+=submodules/freertos-addons/c++/Source/csemaphore.cpp
CXXSRC+=submodules/freertos-addons/c++/Source/cthread.cpp
CXXSRC+=submodules/freertos-addons/c++/Source/cmutex.cpp

# ASM source files
ASRC=startup/startup_stm32f411xe.s

# include directories
INCDIRS=Drivers/CMSIS/Include
INCDIRS+=Drivers/CMSIS/Device/ST/STM32F4xx/Include
INCDIRS+=Drivers/STM32F4xx_HAL_Driver/Inc
INCDIRS+=Middlewares/ST/STM32_USB_Device_Library/Core/Inc
INCDIRS+=Middlewares/USBMIDI/Inc
INCDIRS+=Src
INCDIRS+=Src/configs
INCDIRS+=submodules/etl/include
INCDIRS+=submodules/FreeRTOS-Kernel/include
INCDIRS+=submodules/freertos-addons/c++/Source/include
INCDIRS+=Drivers/STM32F4xx_HAL_Driver/Inc/Legacy

# search path for .so and .a
LIBDIRS=

# libraries to link
LIBS=

# Linker script
LDSCRIPT=STM32F411RCTx_FLASH.ld

# Tool configuration
CROSS=arm-none-eabi-
CC=$(CROSS)gcc
CXX=$(CROSS)g++
AS=$(CROSS)gcc
LD=$(CROSS)g++
OBJCOPY=$(CROSS)objcopy

# Architecture configuration
ARCH_FLAGS=-mcpu=cortex-m4 -DUSE_HAL_DRIVER -DSTM32F411xE -mthumb -specs=nano.specs -specs=nosys.specs -mfloat-abi=hard -mfpu=fpv4-sp-d16 

# Flags for gcc
CFLAGS+=-O0 -ggdb3
CFLAGS+=$(ARCH_FLAGS)
#CFLAGS+=-flto
CFLAGS+=-ffunction-sections
CFLAGS+=-fdata-sections
CFLAGS+=$(foreach i, $(INCDIRS), -I$(i))

# Flags for g++
CXXFLAGS=$(CFLAGS)
CXXFLAGS+=-fno-rtti -fno-exceptions -fno-strict-aliasing
CXXFLAGS+=-std=c++11
CXXFLAGS+=-DCPP_FREERTOS_NO_EXCEPTIONS
CXXFLAGS+=-DCPP_FREERTOS_NO_CPP_STRINGS

# Flags for gcc as linker
LDFLAGS=$(ARCH_FLAGS)
LDFLAGS+=-Wl,--gc-sections
LDFLAGS+=$(foreach i, $(LIBDIRS), -L$(i))
LDFLAGS+=-T $(LDSCRIPT)

# No man land! Enter at your risk

OBJS=$(SRC:.c=.o) $(ASRC:.s=.o) $(CXXSRC:.cpp=.o)
DEPS=$(OBJS:.o=.d)

# Default make target (first target)
all: $(TARGET) bins

# include dependency files (*.d)
-include $(DEPS)

# Rules to build c/cpp/s files
%.o: %.c
	@echo " CC $<"
	@$(CC) -MMD $(CFLAGS) -o $@ -c $<

%.o: %.cpp
	@echo " CXX $<"
	@$(CXX) -MMD $(CXXFLAGS) -o $@ -c $<

%.o: %.s
	@echo " AS $<"
	@$(AS) $(CFLAGS) -o $@ -c $<

# Linker rule
$(TARGET): $(OBJS)
	@echo " LINK $@"
	@$(LD) -o $@ $(LDFLAGS) $(OBJS)

# Phony rules (not file-asociated)
.PHONY: clean all libs clean_lib bins

libs:
	@$(MAKE) -C lib

clean_lib:
	@$(MAKE) -C lib clean

clean:
	@echo " CLEAN"
	@rm -fR $(OBJS) $(DEPS) $(TARGET) $(TARGET:.elf=hex) $(TARGET:.elf=.bin)

bins: $(TARGET)
	@echo " BUILD HEX & BIN"
	@$(OBJCOPY) -O ihex $(TARGET) $(TARGET:.elf=.hex) 
