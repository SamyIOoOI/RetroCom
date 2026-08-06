<h1 align="center">RetroCom</h1>

<img src="Graphics\RetroCom Banner.gif" alt="RetroCom Banner">


--------------------------
---------------------------------
## A multiple channel one-to-one (Simultaneous Speech) two-way INTERCOM built around the AT89C52-PI24 MCU // AT89S52-PU24 (8052 Core) with an Embedded FM Radio Model.


### <p align="center">[Main Features](#main-features) **-** [Variations](#variations) **-** [PCB & Schematic](#pcb--schematic-designs) **-** [Assembly Instructions](#-assembly-instructions-) **-** [Case](#case-design) **-** [BOM [Table]](#bill-of-materials) **-** [BOM [CSV]]()</p>

<p align="center">
<img src="Graphics\Small Fusion Banner.png">&nbsp;&nbsp;&nbsp;&nbsp;<img src="Graphics\Small 8052 Banner.png">&nbsp;&nbsp;&nbsp;&nbsp;<img src="Graphics\Small KiCad Banner.png">
</p>

## Main Features

- AT89C52-PI24 MCU per Station. *[Handles Interstation Communication & Interface]*.
- Supports two-way simultaneous speech. No taking turns when talking. RetroCom functions just like a phonecall.
- All stations are connected to main station directly via CAT6 Cables. Distances of 20M (Meters) to 30M are **OK** .
- Switching is done by CD4051 & CD4052 ICs powered with negative & postive 5 V Rails. PreAmped Signals face no distortion.
- Audio undergoes a preamp stage under the TL072 & power amp stage under the LM386.
- The main station has a built in Radio Module. [As of Version 1.0 [WIRED] the radio is restricted to main station with prospects to expand to other stations via HF Radio models in the Wirless versions of RetroCom.]
- All the substations are powered via the Ethernet *(CAT6 if available, other 8 cable variants usable)* No power outlets are needed at the substations.
- Main station supports up to 4 stations in wired mode. Safety measures are applied in the code. No hangs happen when a station is disconnected.
- Code is written in C & compiled with SDCC. Refer to [Station1.hex](station1.hex) & [Station2.hex](station2.hex) for flash-ready code. 

**Be Aware that slight modifications to the code might be required when using different I2C Display modules. Refer to line [35 in main.c](src\main.c) .**

------------------------------------
------------------------------------
## Variations


1- 1.0.0 Wired 2-3-4-Station Package. **[Finished]**:-

- Main station + Station 2 + Station 3 + Station 4
- Code polls for all four stations.
- Main station is connected to the substations via CAT6 Wires.
- Radio control & listen-in is restricted to Station 1 [Main].
- deemed more secure due to the nature of its wired connections.

2- 1.0.0 Wireless 2-30 Station Package [Unfinished]:-
- Planned to dynamically support up to 30 or more stations
- Drops support for CAT6 Wired connections in favor of HF or UHF Radio Transmission.
- Might have security concerns related to signal interception.

-----------------------------------

## PCB & Schematic Designs

The PCB & Schematics were designed in KiCad with a focus on order & accessibility in the case of hand soldering. The RDA5807M [Main Station] is the only SMD Component.``

``All designs follow a two-layer order with the bottom layer functioning as a GND fillzone.``




**== Station 1 [Main Station] [Schematic](KiCad_PCB_Source\RetroCom.kicad_sch) ==**

<img src="Graphics\Schematic Station 1.png">

**== Station 1 [Main Station] [PCB](KiCad_PCB_Source\RetroCom.kicad_pcb) ==**

<img src="Graphics/PCB Station 1.png">

``3D Model of Board - Uses DIP Sockets Instead of IC Models``

<img src="Graphics\3D PCB Station 1.png">

**== Station 2 [Side Station 1] [Schematic](KiCad_PCB_Source\RetroCom_Station2\RetroCom_Station2.kicad_sch) ==**

<img src="Graphics\Schematic Station 2.png">

**== Station 2 [Side Station 1] [PCB](KiCad_PCB_Source\RetroCom_Station2\RetroCom_Station2.kicad_pcb) ==**

<img src="Graphics\PCB Station 2.png">

``3D Model of Board - Uses DIP Sockets Instead of IC Models``

<img src="Graphics\3D PCB Station 2.png">

---------------------------------
## Case Design 

The case design was made in fusion with simplicity & practicality in mind.

<img src="Graphics\RetroCom Case.png">

The design can be accessed via Fusion Share link on your browser through the [link](https://gmail6713823.autodesk360.com/g/shares/SH28cd1QT2badd0ea72b43d7afa1819b979d) provided.

21 x 13 x 5 (cm) are the dimensions of the Case.

---------------------------------
-----------------------------

## = Assembly Instructions =

1- Flashing the code (.hex) files into the AT89S52 Chip.

- Place the AT89 on a breadboard & build the  crystal and reset circuits. Refer to the [Schematic](Graphics/Schematic%20Station%202.png) for details. Look for labels "**CRYS**" & "**RST**".
- Connect the ISP lines MOSI, MISO, SCK, RESET & GND. EA pin must be connected to VCC. Do not power the circuit before completion.
- Power the circuit through either: A- External 5V Supply B- USBasp's 5V Pinout.
- Use ProgISP to flash the chip. Select AT89S52 from the drop-down menu.

**[ProgISP](https://www.phippselectronics.com/support/prog-isp-software/) Software & [Zadig](https://zadig.akeo.ie/#) Drivers Installer are required to flash the chips.**

## Bill of Materials


## Credits

[Kicad](https://www.kicad.org/) : Schematic & PCB designs

[Autodesk Fusion](https://www.autodesk.com/ae/products/fusion-360/overview) : 3D Case Design

[Visual Studio Code](https://code.visualstudio.com/) : Logic Code Design

[Embedded IDE For VSCode](https://em-ide.com/en/) : Compiling C files into .hex format

[Notaroomba Cyberboard](https://github.com/notaroomba/cyberboard#key-features) : Inspired readme format

## Licence

Made by SamyIOoOI on github under the GPL-3.0 Licence.
