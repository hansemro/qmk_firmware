# Cooler Master MasterKeys Pro S RGB

A TKL keyboard with per-key RGB lighting.

* Keyboard Maintainer: [hansemro](https://github.com/hansemro)
* Hardware Supported: SGK-6030 v1.3 (with US/ANSI layout)
* Hardware Availability: [Discontinued](https://www.coolermaster.com/catalog/peripheral/keyboards/masterkeys-pro-s/)
* Schematic:
    * [PDF](https://github.com/hansemro/re-masterkeys/blob/Pro_S_RGB/kicad/pros_rgb/pros_rgb_schematic.pdf)
    * [KiCad](https://github.com/hansemro/re-masterkeys/tree/Pro_S_RGB/kicad/pros_rgb)

Hardware details:
* Holtek HT32F1654 (Cortex-M3)
    * 63 kiB embedded flash
    * 16 kiB SRAM
    * LQFP64
    * USB/UART ISP bootrom accessed by shorting SEL2 header pins on reset
        * Interface over USB with `ht32-dfu-tool`
    * USB IAP bootloader (~11 kiB) @ 0x0 of embedded flash
        * Interface over USB with `pok3rtool`
        * Boots firmware at 0x3400
        * Checks 0x3000 for version string
    * SWD header (CN2) on PCB
* 3x Macroblock MBIA043GP LED drivers
* GigaDevice 25Q40CT 4 Mibit SPI flash
* Micro USB port

## Warning: Flashing QMK to locked HT32 will brick your keyboard

From the factory, the HT32 processor comes with flash protection and debug security enabled. These
protections need to be disabled to boot QMK without lockups and leaving the keyboard unresponsive.

* See here for more information on [unlocking HT32 MCU](https://github.com/pok3r-custom/pok3r_re_firmware/wiki/HT32-Unlocking).
* See section below for [unlocking/unbricking](#UnlockingUnbricking)

## How to compile

Make example for this keyboard (after setting up your build environment):

```bash
make masterkeys/prosrgb:default
```

## Installing (on unlocked keyboard)

`pok3rtool` interfaces with the keyboard while running IAP bootloader or stock application firmware.
Rebooting to In-Application Programming (IAP) mode will be necessary for upgrading QMK over USB or
returning to stock firmware.

### Building pok3rtool

1. Install the following build dependencies:
- cmake
- make
- gcc

2. Clone and build a fork of `pok3rtool` with support for Cooler Master devices:

```bash
git clone -b cooler-master-dev https://github.com/hansemro/pok3rtool.git --recursive
cd pok3rtool
mkdir build
cd build
cmake ..
make
```

If successful, `pok3rtool` executable binary should be inside the `build` directory.

### Flashing the firmware

Flash QMK firmware with `pok3rtool` (with appropriate path to firmware binary):

```bash
pok3rtool -t prosrgb flash 1.2.3 masterkeys_prosrgb_default.bin
```

Note: version string is arbitrary but necessary for allowing IAP to boot into application firmware.

## Unlocking/Unbricking

The process of unlocking the HT32 processor involves a flash mass erase procedure followed by
restoring the stock IAP bootloader. The simplest method involves booting to In-System Programming
(ISP) mode and using ht32-dfu-tool.

#### Building and Using ht32-dfu-tool

1. Install latest stable rust toolchain and libusb library with headers.

2. Clone ht32-dfu-tool and build with cargo:

```bash
git clone https://github.com/hansemro/ht32-dfu-tool.git
cd ht32-dfu-tool
cargo build -r
```

3. Short the SEL2 pins on the back of the PCB together then plug in the USB cable. Leave the pins
shorted until the IAP bootloader is flashed.

If successful, the keyboard should be detected by `ht32-dfu-tool`:

```bash
$ ./target/release/ht32-dfu-tool list
Device 0: [04d9:8010] Model=HT32F1654 Bus=3 Port=4 Addr=66
```

With the SEL2 pins shorted, flash the [stock USB IAP bootloader](https://github.com/hansemro/pok3r_re_firmware/raw/cmprosrgb/disassemble/cmprosrgb/builtin/cmprosrgb_builtin.bin):

```bash
./target/release/ht32-dfu-tool -w -r write -m -v 0 ./cmprosrgb_builtin.bin
```

Keyboard should reboot and can now be detected in IAP bootloader mode by `pok3rtool`:

```bash
$ pok3rtool list
List Devices...
MasterKeys Pro S RGB (bootloader): CLEARED
```

4. Flash application firmware (stock or QMK) with `pok3rtool`.

## Entering IAP bootloader

Enter the bootloader in 4 ways:

* **Bootmagic reset**: Hold down the `Esc` key and plug in the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
* **Jump to bootloader via Magic command**: With Command enabled, press `LShift+RShift+B` or `LShift+RShift+Esc`
* **Wipe and reflash bootloader**: See [here](#UnlockingUnbricking)

## Rebooting to FW from IAP bootloader

Use `pok3rtool` to reboot keyboard back to application (AP) firmware:

```
$ `pok3rtool` -t prosrgb reboot
Opened MasterKeys Pro S RGB (bootloader)
Reset to Firmware
true
```
