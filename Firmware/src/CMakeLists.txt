
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
    lcd/84x48_mono/LcdContainer.cpp
    lcd/84x48_mono/Pcd8544.cpp
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
)

target_include_directories(Src PUBLIC 
    .
    ./lcd/84x48_mono
)

target_link_libraries(Src PUBLIC
    Etl
    FreertosWrappers
    StdLib
    StHal
    Usbd
    UsbMidi
)
