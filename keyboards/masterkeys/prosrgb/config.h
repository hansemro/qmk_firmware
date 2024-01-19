// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "pin_defs.h"

#define MATRIX_IO_DELAY 0

#ifdef EEPROM_DRIVER
// SPI Flash
#    define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN B10
#    define EXTERNAL_FLASH_CLOCK_DIVISOR 16
#    define EXTERNAL_FLASH_SIZE (512 * 1024)      /* 512 KiB */
#    define EXTERNAL_FLASH_PAGE_SIZE 256          /* 256 B */
#    define EXTERNAL_FLASH_SECTOR_SIZE 4096       /* 4 KiB */
#    define EXTERNAL_FLASH_BLOCK_SIZE (64 * 1024) /* 64 KiB */

// SPI Configuration
#    define SPI_DRIVER SPID1
#    define SPI_SCK_PIN B7
#    define SPI_MOSI_PIN B8
#    define SPI_MISO_PIN B9
#    define SPI_CS_PIN B10
#endif

/* MBIA043 definition */
#define MBI_NUM_CHANNELS 16
#define MBI_SHIFT_REG_WIDTH 10 /* bits */
#define MBI_DATA_LATCH 1
#define MBI_GLOBAL_LATCH 2
#define MBI_READ_CONFIGURATION 4
#define MBI_WRITE_CONFIGURATION 8
#define MBI_ENABLE_WRITE_CONFIGURATION 18
/* MCU -> MBIA_R -> MBIA_G -> MBIA_B -> MCU */
#define MBI_NUM_CASCADE 3 /* one MBIA IC for each color channel */
#define MBI_LED_TYPE RGB
#define MBI_LED_DIRECTION ROW2COL
#define MBI_PWM_DRIVER PWMD_GPTM1
#define MBI_PWM_CHANNEL 0
#define MBI_TIMER_DRIVER GPTD_BFTM0

/* MBI pins */
#define MBI_GCLK_PIN C0
#define MBI_GCLK_OUTPUT_MODE (PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_HT32_AF(AFIO_TM))
#define MBI_DCLK_PIN A14
#define MBI_DCLK_OUTPUT_MODE (PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_HT32_AF(AFIO_GPIO))
#define MBI_SDI_PIN C2 /* data sent to first MBIA from MCU */
#define MBI_SDI_OUTPUT_MODE (PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_HT32_AF(AFIO_GPIO))
#define MBI_SDO_PIN B0 /* data sent to MCU from last MBIA */
#define MBI_SDO_INPUT_MODE (PAL_MODE_INPUT | PAL_MODE_HT32_AF(AFIO_GPIO))
#define MBI_LE_PIN A15
#define MBI_LE_OUTPUT_MODE (PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_HT32_AF(AFIO_GPIO))
/* 3.3V supply to MBIA ICs is enabled when C1 is low */
#define MBI_POWER_ENABLE_PIN C1
#define MBI_POWER_ACTIVE_LOW 1
#define MBI_POWER_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO))

/* LED ROW GPIO Pins */
#define MBI_LED_GPIO_PINS \
    { C8, C7, B5, B4, B3, B2, C6, C5 }
#define MBI_LED_GPIO_OUTPUT_MODE (PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO))

#define MBIA043_CONFIGURATION 0xc
