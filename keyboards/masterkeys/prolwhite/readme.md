# masterkeys/prolwhite

* Keyboard Maintainer: [hansemro](https://github.com/hansemro)
* Hardware Supported: SGK4070 (with US/ANSI layout)
* Hardware Availability: Discontinued

Make example for this keyboard (after setting up your build environment):

    make masterkeys/prolwhite:default

Building pok3rtool for flashing:

    git clone -b cooler-master-dev https://github.com/hansemro/pok3rtool.git --recursive
    cd pok3rtool
    mkdir -p build
    cd build
    cmake ..
    make

Flashing QMK firmware with pok3rtool:

    ./pok3rtool -t prolwhite flash V1.08.01 masterkeys_prolwhite_default.bin
