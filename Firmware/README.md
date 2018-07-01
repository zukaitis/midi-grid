# Firmware
Firmware for the device is built in [SW4STM32](http://www.openstm32.org/HomePage) IDE, mostly written in C++.

.dfu output file generation is included in post-build steps, which requires [HEX2DFU.exe](https://community.st.com/servlet/JiveServlet/download/146386-1-7472/HEX2DFU_REL001.zip)
command line tool, it can be downloaded from [here](https://community.st.com/thread/37026-dfu-fail-verify-8-byte-align-magic) (.exe file shall be put in the Firmware folder).

## Implementation
### Grid
Whole control of the grid - LED refreshing and button state acquisition - is performed by specifically configured DMA. During one cycle DMA latches column selection outputs,
loads timers with desired PWM output values, and reads button input values. After updating all the matrix, DMA generates interrupt, which indicates, that button input values
should be checked for changes. Using DMA significantly reduces CPU load, as it does not have to be interrupted every 500us to acquire input values and update outputs.

## LCD
LCD is interfaced through SPI, using DMA. LCD backlight is connected on MOSI output of another SPI peripheral, and brightness of backlight is regulated by changing the data, that
is constantly transmitted by backlight SPI controller - this imitates an independent PWM output and does not use processor resources.

## Program
Currently there is only one operating mode, which is used to work with Ableton and Launchpad95g script. In this mode, LED colours are set according to MIDI messages received from
DAW, and corresponding MIDI messages are sent to PC, when any of the buttons is pressed or released.

## USB
Microcontroller is configured as a MIDI USB device, and interfaced using relatively simple MIDI messages.