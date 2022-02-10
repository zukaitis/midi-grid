
# USB device
set(usbd_dir ./ST/STM32_USB_Device_Library/Core)
set(usbd_source_dir ${usbd_dir}/Src)

add_library(Usbd STATIC
    ${usbd_source_dir}/usb_device.c
    ${usbd_source_dir}/usbd_conf.c
    ${usbd_source_dir}/usbd_core.c
    ${usbd_source_dir}/usbd_ctlreq.c
    ${usbd_source_dir}/usbd_desc.c
    ${usbd_source_dir}/usbd_ioreq.c
)

target_include_directories(Usbd PUBLIC ${usbd_dir}/Inc)
target_link_libraries(Usbd PUBLIC
    StHal
    UsbMidi
)

# USB MIDI
add_library(UsbMidi STATIC ./USBMIDI/Src/usbd_midi.c)
target_include_directories(UsbMidi PUBLIC ./USBMIDI/Inc)
target_link_libraries(UsbMidi PRIVATE Usbd)
