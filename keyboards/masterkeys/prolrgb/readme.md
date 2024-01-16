# Cooler Master MasterKeys Pro L RGB

* Keyboard Maintainer: [hansemro](https://github.com/hansemro)
* Hardware Supported: SGK-6020 v1.2 (with US/ANSI layout)
* Hardware Availability: [Discontinued](https://www.coolermaster.com/catalog/peripheral/keyboards/masterkeys-pro-l/)

Hardware details:
* Holtek HT32F1654 (Cortex-M3)
    * 63 kB embedded flash
    * 16 kB SRAM
    * LQFP64
    * USB/UART ISP bootrom accessed by shorting BOOT1 pin to ground on reset
    * USB IAP bootloader (~10 kB) @ 0x0 of embedded flash
    * SWDIO + SWCLK labeled on PCB
* 3x Macroblock MBI5042GP LED drivers
* Macronix 25L4006E 4 Mb SPI flash

## Warning: Flashing QMK to locked HT32 will brick your keyboard

From the factory, the HT32 processor comes with flash protection and debug security enabled. These protections need to be disabled to boot QMK without lockups and leaving the keyboard unresponsive.

* See here for more information on [unlocking HT32 MCU](https://github.com/pok3r-custom/pok3r_re_firmware/wiki/HT32-Unlocking).
* See section below for [unlocking/unbricking](#UnlockingUnbricking)
* See here for [unlocking with Raspberry Pi 4](https://github.com/mateuszradomski/re-masterkeys/issues/1#issuecomment-1143137173).

## How to compile

Make example for this keyboard (after setting up your build environment):

```bash
make masterkeys/prolrgb:<keymap>
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
pok3rtool -t prolrgb flash 1.2.3 masterkeys_prolrgb_<keymap>.bin
```

## Unlocking/Unbricking

The process of unlocking the HT32 processor involves a flash mass erase procedure followed by restoring the stock IAP bootloader.

Of the two methods available to unlock the processor, the ISP method is recommended as it does not require a JTAG debugger.

### with ISP/Pin-Shorting Method (Recommended)

#### Building ht32-dfu-tool

```bash
git clone https://github.com/hansemro/ht32-dfu-tool.git
cd ht32-dfu-tool
cargo build -r
```

Short the SEL2 pins on the back of the PCB together then plug in the USB cable. Leave the pins shorted until IAP bootloader is flashed.

If successful, the keyboard should be detected by `ht32-dfu-tool`:

```bash
$ ./target/release/ht32-dfu-tool list
Device 0: [04d9:8010] Model=HT32F1654 Bus=3 Port=4 Addr=66
```

Flash [stock USB IAP bootloader](https://github.com/hansemro/pok3r_re_firmware/raw/cmprolrgb/disassemble/cmprolrgb/builtin/cmprolrgb_builtin.bin):

```bash
./target/release/ht32-dfu-tool -w -r write -m -v 0 ./cmprolrgb_builtin.bin
```

Keyboard should now be detected in IAP bootloader mode by `pok3rtool`:

```bash
$ pok3rtool list
List Devices...
MasterKeys Pro L RGB (bootloader): CLEARED
```

### w/ SWD Debugger and OpenOCD

Mass erase with [openocd fork with HT32 support](https://github.com/hansemro/openocd-ht32/tree/ht32f165x-dev):

```
> halt; ht32f165x mass_erase 0; reset halt
```

Flash [stock USB IAP bootloader](https://github.com/hansemro/pok3r_re_firmware/raw/cmprolrgb/disassemble/cmprolrgb/builtin/cmprolrgb_builtin.bin):

```
> flash write_image ./cmprolrgb_builtin.bin 0; reset
```

Keyboard should now be detected in IAP bootloader mode by pok3rtool:

```bash
$ pok3rtool list
List Devices...
MasterKeys Pro L RGB (bootloader): ERROR
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
$ pok3rtool -t prolrgb reboot
Opened MasterKeys Pro L RGB (bootloader)
Reset to Firmware
true
```
