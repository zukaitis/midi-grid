
# Configs
add_library(Configs INTERFACE)
target_include_directories(Configs INTERFACE ./configs)

# Source

add_library(Src STATIC
    additional_buttons/AdditionalButtons.cpp
    application/Application.cpp
    application/grid_test/GridTest.cpp
    application/internal_menu/InternalMenu.cpp
    application/launchpad/Launchpad.cpp
    application/launchpad/LcdGui.cpp
    application/snake/Snake.cpp
    application/startup/Startup.cpp
    grid/ButtonInput.cpp
    grid/FlashingLeds.cpp
    grid/Grid.cpp
    grid/GridContainer.cpp
    grid/LedOutput.cpp
    grid/PulsingLeds.cpp
    hardware/grid/GridDriver.cpp
    hardware/lcd/BacklightDriver.cpp
    hardware/lcd/Spi.cpp
    lcd/240x320/lcd/LcdContainer.cpp
    lcd/240x320/lcd/Ili9341.cpp
    lcd/240x320/lcd/Pixel.cpp
    lcd/backlight/Backlight.cpp
    lcd/font/nokia_8p.cpp
    lcd/font/monoton_80p.cpp
    lcd/font/rubik_16p.cpp
    lcd/font/rubik_24p.cpp
    lcd/font/sarpanch_79p.cpp
    lcd/image/Image.cpp
    lcd/images/circle_23x23.cpp
    lcd/images/circle_empty_23x23.cpp
    lcd/images/down_arrow_9x23.cpp
    lcd/images/midi_23x23.cpp
    lcd/images/triangle_21x23.cpp
    lcd/images/up_arrow_9x23.cpp
    lcd/images/usb_41x23.cpp
    lcd/images/usb_240x107.cpp
    lcd/shapes/Shapes.cpp
    lcd/text/Font.cpp
    lcd/text/Format.cpp
    lcd/text/Text.cpp
    lcd/backlight/Backlight.cpp
    lcd/Lcd.cpp
    lcd/PriorityManager.cpp
    rotary_controls/RotaryControls.cpp
    syslog/Log.cpp
    syslog/LogContainer.cpp
    syslog/LogThread.cpp
    syslog/Storage.cpp
    system/GlobalInterrupts.cpp
    system/stm32f4xx_it.c
    system/system_stm32f4xx.c
    system/System.cpp
    testing/Testing.cpp
    types/Color.cpp
    usb/UsbMidi.cpp
    utilities/atoi.cpp
)

target_include_directories(Src PUBLIC 
    .
    ./lcd/240x320
)

target_link_libraries(Src PUBLIC
    Etl
    FreertosWrappers
    StHal
    Usbd
    UsbMidi
)
