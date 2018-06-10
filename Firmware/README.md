# Firmware
Firmware for the device is built in [SW4STM32](http://www.openstm32.org/HomePage) IDE, mostly written in C++.

I've included .dfu output file generation in post-build steps, which requires [HEX2DFU.exe](https://community.st.com/thread/37026-dfu-fail-verify-8-byte-align-magic) command line tool, you can get it [here](https://community.st.com/thread/37026-dfu-fail-verify-8-byte-align-magic) (.exe file shall be put in the Firmware folder).
