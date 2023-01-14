# Vortex POK3R RGB

A 60% keyboard with per-key RGB lighting.

* Keyboard Maintainer: [hansemro](https://github.com/hansemro)
* Hardware Supported: `CYKB167_D_V03` (with US/ANSI layout)
* Hardware Availability: Discontinued

Hardware details:
* Holtek HT32F1654 (Cortex-M3)
    * 63 kiB embedded flash
    * 16 kiB SRAM
    * LQFP64
    * USB/UART ISP bootrom accessed by shorting SEL3 header pins on reset
    * USB IAP bootloader (~9 kiB) @ 0x0 of embedded flash
    * SWD header (CN3) on PCB
* 2x Macroblock MBIA043GP LED drivers
* GigaDevice 25Q41BT 4 Mibit SPI flash

## Warning: Flashing QMK to locked HT32 will brick your keyboard

From the factory, the HT32 processor comes with flash protection and debug security enabled. These protections need to be disabled to boot QMK without lockups and leaving the keyboard unresponsive.

* See here for more information on [unlocking HT32 MCU](https://github.com/pok3r-custom/pok3r_re_firmware/wiki/HT32-Unlocking).
* See section below for [unlocking/unbricking](#UnlockingUnbricking)
* See here for [unlocking with Raspberry Pi 4](https://github.com/mateuszradomski/re-masterkeys/issues/1#issuecomment-1143137173).

## How to compile

Make example for this keyboard (after setting up your build environment):

```bash
make vortex/pok3r_rgb:<keymap>
```
## Installing (on unlocked keyboard)

### Building pok3rtool

`pok3rtool` interfaces with the keyboard while running stock bootloader or stock application firmware. Rebooting to bootloader mode will be necessary for upgrading QMK over USB or returning to stock firmware.

```bash
git clone https://github.com/pok3r-custom/pok3rtool.git --recursive
cd pok3rtool
mkdir build
cd build
cmake ..
make
```

### Flashing the firmware

Flash QMK firmware with pok3rtool (use appropriate path to firmware binary):

```bash
pok3rtool -t pok3r_rgb flash V1.03.01 vortex_pok3r_rgb_<keymap>.bin
```

## Unlocking/Unbricking

The process of unlocking the HT32 processor involves a flash mass erase procedure followed by restoring the stock IAP bootloader.

Of the two methods available to unlock the processor, the ISP method is recommended as it does not require a JTAG debugger.

### with ISP/Pin-Shorting Method (Recommended)

* As of January 2023, Holtek ISP protocol support has not yet been merged to upstream `pok3rtool` repo.
    * `git clone https://github.com/hansemro/pok3rtool -b holtek-isp-libusb-dev pok3rtool-isp`

Short the SEL2 pins on the back of the PCB together then plug in the USB cable. Leave the pins shorted until IAP bootloader is flashed.

If successful, the keyboard should be detected by `pok3rtool` as `Holtek ISP USB`:

```
$ pok3rtool list
List Devices...
Holtek ISP USB: 100
```

Flash [stock USB IAP bootloader](https://github.com/pok3r-custom/pok3r_re_firmware/raw/master/disassemble/pok3r_rgb/builtin_rgb/firmware_builtin_rgb.bin):

```
$ pok3rtool -t isp flash 0 ./firmware_builtin_rgb.bin
```

Keyboard should now be detected in IAP bootloader mode by pok3rtool:

```bash
$ pok3rtool list
List Devices...
Vortex POK3R RGB: CLEARED
```

### with SWD Debugger and OpenOCD

Mass erase with [openocd fork with HT32 support](https://github.com/hansemro/openocd-ht32/tree/ht32f165x-dev):

```
> halt; ht32f165x mass_erase 0; reset halt
```

Flash [stock USB IAP bootloader](https://github.com/pok3r-custom/pok3r_re_firmware/raw/master/disassemble/pok3r_rgb/builtin_rgb/firmware_builtin_rgb.bin):

```
> flash write_image ./firmware_builtin_rgb.bin 0; reset
```

Keyboard should now be detected in IAP bootloader mode by pok3rtool:

```bash
$ pok3rtool list
List Devices...
Vortex POK3R RGB: CLEARED
```

## Entering IAP bootloader

Enter the bootloader in 4 ways:

* **Bootmagic reset**: Hold down the `Esc` key and plug in the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
* **Jump to bootloader via Magic command**: With Command enabled, press `LShift+RShift+B` or `LShift+RShift+Esc`
* **Wipe and reflash bootloader**: See [here](#UnlockingUnbricking)

## Rebooting to FW from IAP bootloader

Use pok3rtool to reboot keyboard back to application (AP) firmware:

```
$ pok3rtool -t pok3r_rgb reboot
Opened Vortex POK3R RGB
Reset to Firmware
true
```
