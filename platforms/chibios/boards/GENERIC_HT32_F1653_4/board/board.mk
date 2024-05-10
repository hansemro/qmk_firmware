include $(CHIBIOS_CONTRIB)/os/hal/boards/HT_HT32F1653_4/board.mk

# Required include directories
BOARDINC += $(BOARD_PATH)/board

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
