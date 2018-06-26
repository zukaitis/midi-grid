# Hardware
PCB project is made using [Kicad](http://kicad-pcb.org/), everything, including 3D representation should work right out of the box. Bill of materials, with links to most of the components used, is provided in [BOM.ods](https://github.com/zukaitis/midi-grid/blob/master/Hardware/BOM.ods) file.

PCB of design version v1.0 was assembled and tested, some changes were made, they are documented in [changelog.txt](https://github.com/zukaitis/midi-grid/blob/master/Hardware/Fabrication%20outputs/v1.1/changelog.txt) file.

As far as I tested, 10k resistors connected to gates of MOSFETs are not required, and can be left unassembled. Required values of resistors R25-R28, R33-R36 and R41-R44 will vary depending on the LEDs you use, so calculating appropriate resistor values yourself is advised here.

![3D render](https://github.com/zukaitis/midi-grid/blob/master/Images/pcb_3d_render_front.png)
