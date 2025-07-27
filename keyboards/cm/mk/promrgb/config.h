// Copyright 2025 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next "config.h"

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
