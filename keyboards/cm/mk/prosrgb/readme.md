# Cooler Master MasterKeys Pro S RGB

A TKL keyboard with per-key RGB lighting.

* Keyboard Maintainer: [hansemro](https://github.com/hansemro)
* Hardware Supported: SGK-6030 v1.3 (with ANSI or ISO layouts)
* Hardware Availability: [Discontinued](https://www.coolermaster.com/catalog/peripheral/keyboards/masterkeys-pro-s/)
* Schematic:
    * [PDF](https://github.com/hansemro/re-masterkeys/blob/Pro_S_RGB/kicad/pros_rgb/pros_rgb_schematic.pdf)
    * [KiCad](https://github.com/hansemro/re-masterkeys/tree/Pro_S_RGB/kicad/pros_rgb)

Hardware details:
* Holtek HT32F1654 (Cortex-M3)
    * 63 KiB program flash (+ 1 KiB option bytes)
    * 16 KiB SRAM
    * LQFP64
    * USB/UART ISP bootrom accessed by shorting SEL2 header pins on reset
        * Interfaces over USB with `ht32-dfu-tool`
    * SWD header (CN2) on PCB
* 3x Macroblock MBIA043GP LED drivers
* GigaDevice 25Q40CT 4 Mibit SPI flash
* Micro USB port

## How to compile

Make example for this keyboard with the ANSI layout (after setting up your build environment):

    make cm/mk/prosrgb/ansi:default

For this keyboard with the ISO layout:

    make cm/mk/prosrgb/iso:default

> [!WARNING]
> Flashing onto keyboard with flash security may result in a soft brick. The following example will
> only work after a mass erase to clear flash page protection and flash security. See section below
> on [unlocking/unbricking](#UnlockingUnbricking) for steps to mass erase and flash QMK.

Flashing example for **unlocked** keyboard (with ANSI layout):

    make cm/mk/prosrgb/ansi:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Unlocking/Unbricking

The process of unlocking the HT32 processor involves a flash mass erase procedure. The simplest
method involves booting to In-System Programming (ISP) mode and using `ht32-dfu-tool`.

### with ISP/Pin-Shorting Method

#### Building and Using ht32-dfu-tool

1. Install latest stable rust toolchain and libusb library with headers.

2. Install `ht32-dfu-tool` with cargo from upstream git repo:

```bash
cargo install --git https://github.com/hansemro/ht32-dfu-tool
```

3. Short the SEL2 pins on the back of the PCB together then plug in the keyboard.

If successful, the keyboard should boot to ISP and be detected by `ht32-dfu-tool`:

```bash
$ ht32-dfu-tool list
Device 0: [04d9:8010] Model=HT32F1654 Bus=3 Port=4 Addr=66
```

4. With the SEL2 pins shorted, flash QMK binary firmware with mass-erase flag (`-m`).

> [!NOTE]
> A reset is performed after mass erase, so keep SEL2 pins shorted to re-enter ISP.

> [!WARNING]
> Mass erase will erase stock In-Application (IAP) bootloader and firmware, so proceed at your own
> risk.

```bash
# For ANSI with default keymap
$ ht32-dfu-tool -w -r write -m 0 /path/to/cm_mk_prosrgb_ansi_default.bin
# For ISO with default keymap
$ ht32-dfu-tool -w -r write -m 0 /path/to/cm_mk_prosrgb_iso_default.bin
```

If successful, the keyboard should be unlocked and reboot into QMK.

## Entering ISP bootloader

Enter the bootloader in 4 ways:

* **Bootmagic reset**: Hold down the `Esc` key and plug in the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
* **Jump to bootloader via Magic command**: With Command enabled, press `LShift+RShift+B` or `LShift+RShift+Esc`
* **Configure BOOT1 pin and reset**: Short `SEL2` header pins together (with a tweezer) and plug in the keyboard
