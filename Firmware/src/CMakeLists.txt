
# Configs
add_library(Configs INTERFACE)
target_include_directories(Configs INTERFACE ./configs)

# Source

add_library(Src STATIC
    application/Application.cpp
    application/grid_test/GridTest.cpp
    application/internal_menu/InternalMenu.cpp
    application/launchpad/Launchpad.cpp
    application/launchpad/LcdGui.cpp
    application/snake/Snake.cpp
    application/startup/Startup.cpp
    hardware/grid/GridDriver.cpp
    hardware/lcd/BacklightDriver.cpp
    hardware/lcd/Spi.cpp
    additional_buttons/AdditionalButtons.cpp
    grid/ButtonInput.cpp
    grid/FlashingLeds.cpp
    grid/Grid.cpp
    grid/GridContainer.cpp
    grid/LedOutput.cpp
    grid/PulsingLeds.cpp
    lcd/Lcd.cpp
    lcd/PriorityManager.cpp
    lcd/84x48_mono/LcdContainer.cpp
    lcd/84x48_mono/Pcd8544.cpp
    lcd/backlight/Backlight.cpp
    syslog/Log.cpp
    syslog/LogContainer.cpp
    syslog/LogThread.cpp
    syslog/Storage.cpp
    rotary_controls/RotaryControls.cpp
    testing/Testing.cpp
    usb/UsbMidi.cpp
    main.cpp
    system/GlobalInterrupts.cpp
    system/System.cpp
    system/stm32f4xx_it.c
    system/system_stm32f4xx.c
    types/Color.cpp
    usb/usbd_conf.c
    usb/usbd_desc.c
    usb/usb_device.c
)

target_include_directories(Src PUBLIC 
    .
    ./lcd/84x48_mono
)

target_link_libraries(Src PUBLIC
    StdLib
    Etl
    StHal
    FreertosWrappers
)
