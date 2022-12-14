# Cooler Master MasterKeys Pro L White

* Keyboard Maintainer: [hansemro](https://github.com/hansemro)
* Hardware Supported: SGK-4070 (with US/ANSI layout)
* Hardware Availability: [Discontinued](https://www.coolermaster.com/catalog/peripheral/keyboards/masterkeys-pro-l-white/)
* Schematic:
    * [PDF](https://github.com/hansemro/re-masterkeys/blob/Pro_L_White/kicad/prol_white/prol_white_schematic.pdf)
    * [KiCad](https://github.com/hansemro/re-masterkeys/tree/Pro_L_White/kicad/prol_white)

Hardware details:
* Holtek HT32F52352 (Cortex-M0+)
    * 127 kB embedded flash
    * 16 kB SRAM
    * LQFP64
    * USB IAP bootloader (~10 kB) @ 0x0 of embedded flash
    * SWD header (CN1) on PCB
* Macroblock MBI5042GP LED driver
* Macronix 25L4006E 4 Mb SPI flash

## Warning: Flashing to locked MCU will brick your keyboard

From factory, the HT32 processor comes with flash protection and debug security enabled. These protections need to be disabled (via SWD debugger) to boot QMK without running into hardfaults and leaving the keyboard unresponsive.

* See here for more information on [unlocking HT32 MCU](https://github.com/pok3r-custom/pok3r_re_firmware/wiki/HT32-Unlocking).
* See here for [unlocking with Raspberry Pi 4](https://github.com/mateuszradomski/re-masterkeys/issues/1#issuecomment-1143137173).

## How to compile

Make example for this keyboard (after setting up your build environment):

```bash
make masterkeys/prolwhite:default
```

## Installing (on unlocked keyboard)

### Building pok3rtool

`pok3rtool` interfaces with the keyboard while running stock bootloader or stock application firmware. Rebooting to bootloader mode will be necessary for upgrading QMK over USB or returning to stock firmware.

```bash
git clone -b cooler-master-dev https://github.com/hansemro/pok3rtool.git --recursive
cd pok3rtool
mkdir build
cd build
cmake ..
make
```

### Flashing the firmware

Flash QMK firmware with pok3rtool (use appropriate path to firmware binary):

```bash
pok3rtool -t prolwhite flash V1.08.01 masterkeys_prolwhite_default.bin
```

## Unlocking/Unbricking with SWD debugger and OpenOCD

Mass erase with [openocd fork with HT32 support](https://github.com/hansemro/openocd-ht32/tree/ht32f165x-dev):

```
> halt; ht32f165x mass_erase 0; reset halt
```

Flash [stock USB IAP bootloader](https://github.com/hansemro/pok3r_re_firmware/raw/cmprolwhite/disassemble/cmprolwhite/builtin/cmprolwhite_builtin.bin):

```
> flash write_image ./cmprolwhite_builtin.bin 0; reset
```

Keyboard should now be detected in DFU/IAP bootloader mode by pok3rtool:

```bash
$ pok3rtool list
List Devices...
MasterKeys Pro L White (bootloader): V1.09.00
```

## Rebooting to DFU/IAP bootloader (with default keymap)

While running QMK, press FN+Esc (mapped to `QK_BOOT`) to reboot to bootloader.

## Rebooting to FW from DFU/IAP bootloader

Use pok3rtool to reboot keyboard back to application (AP) firmware:

```
$ pok3rtool -t prolwhite reboot
Opened MasterKeys Pro L White (bootloader)
Reset to Firmware
true
```
