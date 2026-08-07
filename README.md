<h1 align="center">RetroCom</h1>

<img src="Graphics\RetroCom Banner.gif" alt="RetroCom Banner">


--------------------------
---------------------------------
## A multiple channel one-to-one (Simultaneous Speech) two-way INTERCOM built around the AT89C52-PI24 MCU // AT89S52-PU24 (8052 Core) with an Embedded FM Radio Model.


### <p align="center">[Main Features](#main-features) **-** [Variations](#variations) **-** [PCB & Schematic](#pcb--schematic-designs) **-** [Assembly Instructions](#-assembly-instructions-) **-** [Case](#case-design) **-** [BOM [Table]](#bill-of-materials) **-** [BOM [CSV]](BOM/BOM.csv) **-** [BOM[XLSX]](BOM/BOM%20EXCEL.xlsx)</p>

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

1- Connecting the PCB & Case

- In the case of **hand-solder** follow the schematic's connections and use the [PCB](Graphics/PCB%20Station%201.png) Design as a reference.
- Use **DIP Sockets** for the ICs (RDA5807M is an exception as it is SMD).
- Connect the ICs & flashed AT89S52s to their respective hand-soldered/printed PCBs.
- Connect the buzzer, speaker, microphone, LEDs, toggle switches, momentary switches & I2C Display through via jumper cables from their respective headers.
- For power, connect half a female jumper wire to the screw terminals of a female DC Socket to wire terminals connector. This can be connected to a **12VDC** 3 & above Amp Wall Adapter.
- Solder the RJ45 Houses in their locations, run CAT6 Cables from each sub-station to main station. 
- For the case, if using the printed design, connect the board to its respective mounting holes via M3 Screws [8mm - 10mm]. 
- Each printed case design has eight board mounting holes to suit both board sizes (Side Stations & Main Station).
- Place the interface components on the surface panel of the case, run jumper cables to the board below. Use the four corner bound M3 screw holes for holding the panel on the bottom case piece. Run the outgoing cables through the back panel.

2- Flashing the code (.hex) files into the AT89S52 Chip.

- Place the AT89 on a breadboard & build the  crystal and reset circuits. Refer to the [Schematic](Graphics/Schematic%20Station%202.png) for details. Look for labels "**CRYS**" & "**RST**".
- Plug the USBasp into you pc/laptop USB port & select it in Zadig's drop-down menu. Install libusb-win32 or libusbK.
- Connect the ISP lines MOSI, MISO, SCK, RESET & GND. EA pin must be connected to VCC. Do not power the circuit before completion.
- Power the circuit through either: A- External 5V Supply B- USBasp's 5V Pinout.
- Use ProgISP to flash the chip. Select AT89S52 from the drop-down menu.

**[ProgISP](https://www.phippselectronics.com/support/prog-isp-software/) Software & [Zadig](https://zadig.akeo.ie/#) Drivers Installer are required to flash the chips.**


## Bill of Materials

| **Part Name**                                      | **Quantity** | **Price**    | **Link**                                                                                                                                                                                                            |
|----------------------------------------------------|--------------|----------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **33pF Capacitor**                                 | 4.00         |  EGP 2.00      | https://makerselectronics.com/product/ceramic-capacitor-33pf-50v/                                                                                                                                                   |
| **10 uF cap**                                      | 6.00         |  EGP 6.00      | https://www.ram-e-shop.com/shop/c-10u25v-capacitor-10uf-25v-6095                                                                                                                                                    |
| **220 uF Cap**                                     | 2.00         |  EGP 2.00      | https://www.ram-e-shop.com/shop/c-220u16v-capacitor-220uf-16v-5654                                                                                                                                                  |
| **4700 uF cap**                                    | 2.00         |  EGP 12.00     | https://www.ram-e-shop.com/shop/c-4700u25v-capacitor-4700uf-25v-6079                                                                                                                                                |
| **47 uF cap**                                      | 7.00         |  EGP 7.00      | https://makerselectronics.com/product/capacitor-47uf-50v/                                                                                                                                                           |
| **100 nF Cap**                                     | 18.00        |  EGP 13.50     | https://www.ram-e-shop.com/shop/c-pf-104-ceramic-capacitor-pf104-100nf-25v-6240                                                                                                                                     |
| **100 pF Cap**                                     | 1.00         |  EGP 0.50      | https://makerselectronics.com/product/ceramic-capacitor-100pf-50v/                                                                                                                                                  |
| **Yellow LED  10mm**                               | 2.00         |  EGP 6.00      | https://makerselectronics.com/product/yellow-led-transparent-10mm/                                                                                                                                                  |
| **Green LED  10mm**                                | 2.00         |  EGP 6.00      | https://makerselectronics.com/product/green-led-10mm-transparent/                                                                                                                                                   |
| **Red LED  10mm**                                  | 2.00         |  EGP 6.00      | https://makerselectronics.com/product/red-led-transparent-10mm/                                                                                                                                                     |
| **Orange LED  10mm**                               | 2.00         |  EGP 6.00      | https://makerselectronics.com/product/orange-led-10mm-transparent/                                                                                                                                                  |
| **RJ8-P-PCB (RJ45 8P8C Plastic Housing)**          | 5.00         |  EGP 25.00     | https://www.ram-e-shop.com/shop/rj8-p-pcb-rj45-8-pin-connector-on-pcb-plastic-housing-5749                                                                                                                          |
| **10K R**                                          | 14.00        |  EGP 4.00      | https://www.ram-e-shop.com/shop/carbon-resistance-1-4w-price-per-4-resistors-9506#attr=314                                                                                                                          |
| **2.2K R**                                         | 2.00         |  EGP 0.10      | https://makerselectronics.com/product/carbon-resistor-2-2k%cf%89-0-125w-through-hole/                                                                                                                               |
| **10 R**                                           | 2.00         |  EGP 1.50      | https://makerselectronics.com/product/carbon-resistor-10%cf%89-0-25w-through-hole/                                                                                                                                  |
| **470 R**                                          | 8.00         |  EGP 2.00      | https://www.ram-e-shop.com/shop/carbon-resistance-1-4w-price-per-4-resistors-9506#attr=307                                                                                                                          |
| **47k R**                                          | 1.00         |  EGP 1.50      | https://makerselectronics.com/product/carbon-resistor-47k%cf%89-0-25w/                                                                                                                                              |
| **100k R**                                         | 2.00         |  EGP 1.00      | https://www.ram-e-shop.com/shop/carbon-resistance-1-4w-price-per-4-resistors-9506#attr=317                                                                                                                          |
| **47 R**                                           | 2.00         |  EGP 0.20      | https://makerselectronics.com/product/carbon-resistor-47%cf%89-0-125w-through-hole/                                                                                                                                 |
| **220K R**                                         | 2.00         |  EGP 1.50      | https://makerselectronics.com/product/carbon-resistor-220k%cf%89-0-25w-through-hole/                                                                                                                                |
| **4.7K R**                                         | 4.00         |  EGP 1.50      | https://makerselectronics.com/product/carbon-resistor-4-7k-0-25w/                                                                                                                                                   |
| **C8050**                                          | 4.00         |  EGP 3.00      | https://www.ram-e-shop.com/shop/c8050-c8050-6050                                                                                                                                                                    |
| **11.0592MHz Crystal**                             | 2.00         |  EGP 8.00      | https://www.ram-e-shop.com/shop/o-11-0592mhz-crystal-oscillator-11-0592-mhz-5668                                                                                                                                    |
| **32.768 kHz Crystal**                             | 1.00         |  EGP 3.00      | https://www.ram-e-shop.com/shop/o-32-768khz-crystal-oscillator-32-768-khz-5880                                                                                                                                      |
| **PIN Headers x40 Male**                           | 2.00         |  EGP 6.00      | https://makerselectronics.com/product/pin-headers-male-2-54mm-40-pin-straight-black-11mm/                                                                                                                           |
| **PIN Headers x40 Female**                         | 2.00         |  EGP 10.00     | https://makerselectronics.com/product/pin-headers-female-2-54mm-40-pin/                                                                                                                                             |
| **Speakers 8 ohm 0.25 watts**                      | 2.00         |  EGP 20.00     | https://makerselectronics.com/product/speaker-8%cf%89-0-25w-o-29mm/                                                                                                                                                 |
| **Condenser Microphone**                           | 2.00         |  EGP 12.00     | https://makerselectronics.com/product/condenser-microphone-2-pins/                                                                                                                                                  |
| **I2C LCD 2x16 Modules**                           | 2.00         |  EGP 290.00    | https://www.ram-e-shop.com/shop/lcd-i2c-2x16-blue-iic-i2c-1602-serial-character-lcd-2x16-display-8884                                                                                                               |
| **4 Pin Toggle Switch DSPT**                       | 2.00         |  EGP 80.00     | https://makerselectronics.com/product/toggle-switch-4-pins-15a-e-ten1221-o/                                                                                                                                         |
| **2 Pin Toggle Switch SPST**                       | 2.00         |  EGP 40.00     | https://makerselectronics.com/product/toggle-switch-on-off-2-pin-2a-12mm/                                                                                                                                           |
| **Momentary Toggle presses/switches 8**            | 8.00         |  EGP 52.00     | https://makerselectronics.com/product/push-button-switch-on-off-2pin/                                                                                                                                               |
| **Buzzers**                                        | 2.00         |  EGP 50.00     | https://www.ram-e-shop.com/shop/buzzer12v-big-black-buzzer-big-size-3-24v-piezo-electronic-alarm-6122                                                                                                               |
| **PCB 13x25 cm2 Dotted Holes**                     | 2.00         |  EGP 97.00     | https://www.ram-e-shop.com/shop/pcb23-holes-13x25-pcb-13x25-cm2-dots-holes-sku-pcb23-6681                                                                                                                           |
| **Jumper Wire Cables Male to Male [40 Wires]**     | 2.00         |  EGP 70.00     | https://www.ram-e-shop.com/shop/ph61-mm-20cm-ph61-20cm-male-to-male-40-jumper-wires-set-7094                                                                                                                        |
| **Jumper Wire Cables Female to Female [40 Wires]** | 1.00         |  EGP 35.00     | https://www.ram-e-shop.com/shop/ph62-ff-20cm-ph62-20cm-female-to-female-40-jumper-wires-set-7189                                                                                                                    |
| **Jumper Wire Cables Male to Female [40 Wires]**   | 2.00         |  EGP 70.00     | https://www.ram-e-shop.com/shop/ph60-mf-20cm-ph60-20cm-male-to-female-40-jumper-wires-set-7066                                                                                                                      |
| **Solder Wire**                                    | 1.00         |  EGP 210.00    | https://makerselectronics.com/product/soldering-wire-100gm-alloy70-30-made/                                                                                                                                         |
| **Soldering Iron**                                 | 1.00         |  EGP 195.00    | https://www.ram-e-shop.com/shop/se860-suoer-se860-60w-basic-soldering-iron-ct-360-5600                                                                                                                              |
| **Soldering Iron Stand**                           | 1.00         |  EGP 45.00     | https://makerselectronics.com/product/no-218-soldering-iron-stand/                                                                                                                                                  |
| **Soldering Paste**                                | 1.00         |  EGP 25.00     | https://makerselectronics.com/product/yx-18-flux-soldering-paste/?attribute_size=50g                                                                                                                                |
| **Fares USBasp AVR programmer**                    | 1.00         |  EGP 175.00    | https://www.ram-e-shop.com/shop/prog-avr-fares-fares-usbasp-avr-programmer-8325                                                                                                                                     |
| **20 Meter CAT6 Cable**                            | 1.00         |  EGP 165.00    | https://makerselectronics.com/product/zlink-cable-cat6-ethernet-patch-cord/                                                                                                                                         |
| **12 VDC Wall Power Adapter**                      | 1.00         |  EGP 135.00    | https://www.ram-e-shop.com/shop/adapter-fixed-12v-3a-wall-power-adapter-12vdc-3a-high-quality-adapter-5573                                                                                                          |
| **Female DC Jack Adapter with Terminal Block**     | 1.00         |  EGP 7.00      | https://makerselectronics.com/product/female-dc-jack-adapter-terminals/                                                                                                                                             |
| **AT89S52-PU24**                                   | 2.00         |  EGP 170.00    | https://makerselectronics.com/product/at89s52-24pu-8kb-4v5-5v-256byte-24/                                                                                                                                           |
| **IC Socket DIP-40**                               | 2.00         |  EGP 10.00     | https://www.ram-e-shop.com/shop/n20-20-ic-socket-n20-20-base-40-pin-5902                                                                                                                                            |
| **IC Socket DIP-16**                               | 3.00         |  EGP 5.25      | https://makerselectronics.com/product/ic-socket-88-base-16-pin/                                                                                                                                                     |
| **CD4051**                                         | 2.00         |  EGP 30.00     | https://www.ram-e-shop.com/shop/4051-ic-4051-single-8-channel-multiplexer-demultiplexer-5551                                                                                                                        |
| **CD4052BE**                                       | 1.00         |  EGP 9.00      | https://makerselectronics.com/product/cd4052be-dual-4-channel-analog/                                                                                                                                               |
| **IC Socket DIP-8**                                | 10.00        |  EGP 7.50      | https://www.ram-e-shop.com/shop/n4-4-ic-socket-n4-4-base-8-pin-5864                                                                                                                                                 |
| **TL072CN**                                        | 2.00         |  EGP 34.00     | https://www.ram-e-shop.com/shop/tl072-tl072cn-5722                                                                                                                                                                  |
| **LM386N**                                         | 2.00         |  EGP 12.00     | https://www.ram-e-shop.com/shop/lm386-lm386n-5927                                                                                                                                                                   |
| **ICL7660**                                        | 2.00         |  EGP 102.60    | https://uge-one.com/product/icl7660/                                                                                                                                                                                |
| **MAX485CPA+**                                     | 4.00         |  EGP 72.00     | https://makerselectronics.com/product/ic-max485cpa-low-power-transceiver-dip-8/                                                                                                                                     |
| **LM7805 Regulator**                               | 2.00         |  EGP 10.00     | https://makerselectronics.com/product/lm7805-voltage-regulator-to-220-5v/                                                                                                                                           |
| **LD33CV Regulator**                               | 1.00         |  EGP 20.00     | https://www.ram-e-shop.com/shop/ld1117v33-ld1117v33-ld33cv-6735                                                                                                                                                     |
| **TO-220 Regulator Heat Sink**                     | 3.00         |  EGP 10.50     | https://www.ram-e-shop.com/shop/hs2-aluminum-heatsink-hs-2-size-25x14x10-mm-5981                                                                                                                                    |
| **Logic level Converter Module**                   | 1.00         |  EGP 28.50     | https://uge-one.com/product/bi-directional-4-way-level-conversion-module/?gad_source=1&gad_campaignid=24034682861&gclid=CjwKCAjw4dDTBhAqEiwAkHYmSi15wnBhh5vjV9PTNDUeKMSZ1iojFyjaDYYeGKExxMUaX75Kg8KlMhoCpS4QAvD_BwE |
| **Metal Potentiometer 10K? 3-Pin**                 | 2.00         |  EGP 11.00     | https://makerselectronics.com/product/metal-potentiometer-10k%cf%89-3-pin/                                                                                                                                          |
| **RDA5807M**                                       | 1.00         |  EGP 55.00     | https://uge-one.com/product/rda5807m-fm-radio-receiver-smd-ic-msop1                                                                                                                                                 |
| ****                                               |              |  EGP 2,495.65  |                                                                                                                                                                                                                     |
| ****                                               |              |                |                                                                                                                                                                                                                     |
| **Note: Most resistors come in pack of 10s - 4s**  |              |                |                                                                                                                                                                                                                     |
| ****                                               |              |                |                                                                                                                                                                                                                     |
| **Shipping: RAM**                                  |              |  EGP 105.00    |                                                                                                                                                                                                                     |
| **Shipping: Makers Electronics**                   | 80 - 75      |  EGP 80.00     |                                                                                                                                                                                                                     |
| **Shipping: UGE Electronics**                      |              |  EGP 188.10    |                                                                                                                                                                                                                     |
| **VAT: UGE Electronics**                           |              |  EGP 45.95     |                                                                                                                                                                                                                     |
| ****                                               |              |  EGP 419.05    |                                                                                                                                                                                                                     |
| ****                                               |              |                |                                                                                                                                                                                                                     |
| ****                                               |              |  EGP 2,914.70  |                                                                                                                                                                                                                     |


## Credits

[Kicad](https://www.kicad.org/) : Schematic & PCB designs

[Autodesk Fusion](https://www.autodesk.com/ae/products/fusion-360/overview) : 3D Case Design

[Visual Studio Code](https://code.visualstudio.com/) : Logic Code Design

[Embedded IDE For VSCode](https://em-ide.com/en/) : Compiling C files into .hex format

[Notaroomba Cyberboard](https://github.com/notaroomba/cyberboard#key-features) : Inspired readme format

[tableconvert.com](https://tableconvert.com/excel-to-markdown) : Turned my xlsx table to markdown format

## Licence

Made by SamyIOoOI on github under the GPL-3.0 Licence.
