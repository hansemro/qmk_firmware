# MCU
MCU = cortex-m0plus
ARMV = 6
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F523xx
MCU_LDSCRIPT = HT32F52352_PROLWHITE
MCU_STARTUP = ht32f523xx

OPT_DEFS = -DCORTEX_VTOR_INIT=0x3200 -DCRT0_VTOR_INIT

SRC += mbi5042.c
