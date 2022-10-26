# MCU
MCU = cortex-m3
ARMV = 7
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F165x
MCU_LDSCRIPT = HT32F1654_PROSRGB
MCU_STARTUP = ht32f165x

BOARD = SGK_6030_V1_3

# Custom RGB matrix handling
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = custom

# Keys
CUSTOM_MATRIX = lite
KEY_LOCK_ENABLE = no

# Wear-leveling driver
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = spi_flash

# Other features
RAW_ENABLE = no
MIDI_ENABLE = no
VIRTSER_ENABLE = no
COMBO_ENABLE = no

LAYOUTS = tkl_ansi

SRC += matrix.c
SRC += mbia043.c
