/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#pragma once

#include_next <config.h>

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

/* MCU -> MBIA_R -> MBIA_G -> MBIA_B -> MCU */
#define MBIA043_NUM_CASCADE 2 /* one MBIA IC for each color channel */
/* MBIA pins */
#define MBIA043_GCLK_PIN C0
#define MBIA043_DCLK_PIN A14
#define MBIA043_SDI_PIN C2 /* data sent to first MBIA from MCU */
#define MBIA043_SDO_PIN B0 /* data sent to MCU from last MBIA */
#define MBIA043_LE_PIN A15
/* MBIA 5V power enable */
#define MBIA043_HAS_POWER_PIN
/* 5V supply to MBIA ICs is enabled when C1 is low */
#define MBIA043_PWRCTRL_PIN C1

/* LED COL GPIO Pins */
#define LED_COL_PINS \
    { C8, C7, B5, B4, B3, B2, C6, C5 }

#define MBIA043_CONFIGURATION \
    { 0xc, 0xc }
