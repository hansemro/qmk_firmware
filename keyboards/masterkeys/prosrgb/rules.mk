# MCU
MCU = cortex-m3
ARMV = 7
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F165x
MCU_LDSCRIPT = HT32F1654_PROSRGB
MCU_STARTUP = ht32f165x

# Keys
CUSTOM_MATRIX = lite

# Wear-leveling driver
#EEPROM_DRIVER = wear_leveling
#WEAR_LEVELING_DRIVER = spi_flash

SRC += matrix.c
SRC += mbia043.c
