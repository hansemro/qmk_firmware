# Cidoo V65 R1

![V65](https://i.imgur.com/Aht1zfu.png)

A 65% keyboard, which controlled by an AT32F415 chipset. The keyboard feature RGB Matrix, rotary encoder, SPI Flash for EEPROM, Bluetooth.

* Keyboard Maintainer: [HorrorTroll](https://github.com/HorrorTroll)
* Hardware Supported: AT32F415
* Hardware Availability: No longer available

Make example for this keyboard (after setting up your build environment):

    make cidoo/v65/rev1:default

Flashing example for this keyboard:

    make cidoo/v65/rev1:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (Esc key) and plug in the keyboard
* **Physical reset pad**: Use a tweezer and touching 3.3V and BOOT0 pad on the back of the PCB
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
