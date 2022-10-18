# masterkeys/prosrgb

A TKL (87-key) keyboard with per-key RGB lighting.

* Keyboard Maintainer: [hansemro](https://github.com/hansemro)
* Hardware Supported: SGK 6030 v1.3 (with US/ANSI layout)
* Hardware Availability: Discontinued

Make example for this keyboard (after setting up your build environment):

    make masterkeys/prosrgb:default

Building pok3rtool for flashing:

    git clone -b cooler-master-dev https://github.com/hansemro/pok3rtool.git --recursive
    cd pok3rtool
    mkdir -p build
    cd build
    cmake ..
    make

Flashing QMK firmware with pok3rtool:

    ./pok3rtool -t prosrgb flash 1.2.3 masterkeys_prosrgb_debug.bin
