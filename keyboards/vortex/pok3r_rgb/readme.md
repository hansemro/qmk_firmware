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
* Mini-USB port

## How to compile

Make example for this keyboard (after setting up your build environment):

    make vortex/pok3r_rgb:default

> [!WARNING]
> Flashing onto keyboard with flash security may result in a soft brick. The following example will
> only work after a mass erase to clear flash page protection and flash security. See section below
> on [unlocking/unbricking](#UnlockingUnbricking) for steps to mass erase and flash QMK.

Flashing example for **unlocked** keyboard:

    make vortex/pok3r_rgb:default:flash

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

3. Short the SEL3 pins on the back of the PCB together then plug in the keyboard.

If successful, the keyboard should boot to ISP and be detected by `ht32-dfu-tool`:

```bash
$ ht32-dfu-tool list
Device 0: [04d9:8010] Model=HT32F1654 Bus=3 Port=4 Addr=66
```

4. With the SEL3 pins shorted, flash QMK binary firmware with mass-erase flag (`-m`).

> [!NOTE]
> A reset is performed after mass erase, so keep SEL3 pins shorted to re-enter ISP.

> [!WARNING]
> Mass erase will erase stock In-Application (IAP) bootloader and firmware, so proceed at your own
> risk.

```bash
$ ht32-dfu-tool -w -r write -m 0 /path/to/vortex_pok3r_rgb_default.bin
```

If successful, the keyboard should be unlocked and reboot into QMK.

## Entering ISP bootloader

Enter the bootloader in 4 ways:

* **Bootmagic reset**: Hold down the `Esc` key and plug in the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
* **Jump to bootloader via Magic command**: With Command enabled, press `LShift+RShift+B` or `LShift+RShift+Esc`
* **Configure BOOT1 pin and reset**: Short `SEL3` header pins together (with a tweezer) and plug in the keyboard
