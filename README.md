[![Release](https://img.shields.io/github/v/release/zukaitis/midi-grid?include_prereleases&logo=github)](https://github.com/zukaitis/midi-grid/releases/latest) [![Build](https://img.shields.io/github/workflow/status/zukaitis/midi-grid/Build?logo=github)](https://github.com/zukaitis/midi-grid/actions?query=workflow%3A%22Build%22) [![Unit Tests](https://img.shields.io/github/workflow/status/zukaitis/midi-grid/Unit%20Tests?label=unit%20tests&logo=github)](https://github.com/zukaitis/midi-grid/actions?query=workflow%3A%22Unit+Tests%22) [![UT coverage](https://img.shields.io/codecov/c/github/zukaitis/midi-grid?logo=codecov)](https://codecov.io/github/zukaitis/midi-grid?branch=master)

# midi-grid
DIY RGB MIDI grid controller

[![Video](https://img.youtube.com/vi/mxI5Q21d1T0/0.jpg)](https://youtu.be/mxI5Q21d1T0 "Demo video")

midi-grid is an open source MIDI controller, main part of which is 8x8 LED illuminated button grid.

Main features of the device:
- 8x8 RGB LED illuminated button matrix
- 17 additional MIDI control buttons, most of them paired with RGB LED indicator
- 1 internal menu button
- 2 rotary encoders, acting as MIDI controls
- Monochrome LCD display with adjustable backlight
- Physical MIDI input and output ports

Currently device is developed to work with [Ableton Live](https://www.ableton.com/en/live/),
specifically with modified [Launchpad95](http://motscousus.com/stuff/2011-07_Novation_Launchpad_Ableton_Live_Scripts/)
MIDI remote script - [Launchpad95g](https://github.com/zukaitis/Launchpad95g). Additional functionality, like stand-alone modes,
or compatibility with other DAWs may be implemented later.

More thorough information, regarding design and implementation, is available in [wiki of this project](https://github.com/zukaitis/midi-grid/wiki).

## Thanks to
- [@mimuz](https://github.com/mimuz) - for sharing [STM32 USB MIDI library](https://github.com/mimuz/mimuz-tuch)
- [@pidcodes](https://github.com/pidcodes) - for providing USB VID/PID pair for this application
- [@hdavid](https://github.com/hdavid) - for making an amazing Ableton Live extension - [Launchpad95](https://github.com/hdavid/Launchpad95)
- [@michaelbecker](https://github.com/michaelbecker) - for making [FreeRTOS C++ wrappers](https://michaelbecker.github.io/freertos-addons)
