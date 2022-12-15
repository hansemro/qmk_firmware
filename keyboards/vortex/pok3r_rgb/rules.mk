# Vortex POK3R RGB

SRC = \
	ht32.c \
	util.c \
	eeprom_gd25.c \
	mbia043.c

# MCU
MCU = cortex-m3
ARMV = 7
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F165x

# Customized startup file
MCU_STARTUP = ht32f165x
# Customized linker script for application
#MCU_LDSCRIPT = HT32F1654_CYKB
MCU_LDSCRIPT = pok3r_rgb

BOARD = CYKB167_D_V03

#OPT_DEFS = -DCORTEX_VTOR_INIT=0x3400 -Wno-unused-function -fdump-rtl-dfinish -fstack-usage
OPT_DEFS = -DDEBUG_EEPROM_OUTPUT
#EXTRALDFLAGS = -Wl,--print-memory-usage

LAYOUTS = 60_ansi
#LAYOUTS = 60_ansi 60_ansi_split_rshift

# Options

# Keys
#CUSTOM_MATRIX = yes
NKRO_ENABLE = yes
#MOUSEKEY_ENABLE = yes
#EXTRAKEY_ENABLE = no
#KEY_LOCK_ENABLE = no
DIP_SWITCH_ENABLE = yes

# RGB Backlight (uses 2 Macroblock MBIA043 LED driver ICs)
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = custom

EEPROM_DRIVER = custom

# Other featues
#BOOTMAGIC_ENABLE = yes
#VIA_ENABLE = yes
CONSOLE_ENABLE = yes
#COMMAND_ENABLE = yes
#RAW_ENABLE = yes
#MIDI_ENABLE = no
#VIRTSER_ENABLE = no
