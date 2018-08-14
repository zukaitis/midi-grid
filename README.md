# midi-grid
DIY RGB MIDI grid controller

[![Video](https://img.youtube.com/vi/7otjcxH8tro/0.jpg)](https://youtu.be/7otjcxH8tro "Demo video")

midi-grid is an open source MIDI controller, main part of which is 8x8 LED iluminated button grid.

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

More thorough information, regarding design and implementation, is available in READMEs of [Firmware](https://github.com/zukaitis/midi-grid/tree/master/Firmware)
and [Hardware](https://github.com/zukaitis/midi-grid/tree/master/Hardware).

## Thanks to
- [@mimuz](https://github.com/mimuz) - for sharing [STM32 USB MIDI library](https://github.com/mimuz/mimuz-tuch)
- [@pidcodes](https://github.com/pidcodes) - for providing USB VID/PID pair for this application
- [@hdavid](https://github.com/hdavid) - for making an amazing Ableton Live extension - [Launchpad95](https://github.com/hdavid/Launchpad95)
