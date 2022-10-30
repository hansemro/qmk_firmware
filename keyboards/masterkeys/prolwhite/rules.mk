# MCU
MCU = cortex-m0plus
ARMV = 6
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F523xx
MCU_LDSCRIPT = HT32F52352_PROLWHITE
MCU_STARTUP = ht32f523xx

BOARD = PRO_L_WHITE

OPT_DEFS = -DCORTEX_VTOR_INIT=0x3200 -DCRT0_VTOR_INIT

LED_MATRIX_ENABLE = yes
LED_MATRIX_DRIVER = custom

# Keys
CUSTOM_MATRIX = lite
KEY_LOCK_ENABLE = no

# Other features
RAW_ENABLE = no
MIDI_ENABLE = no
VIRTSER_ENABLE = no
COMBO_ENABLE = no

SRC += matrix.c
ifeq ($(LED_MATRIX_ENABLE), yes)
SRC += mbi5042.c
endif
