// Copyright 2025 HorrorTroll <https://github.com/HorrorTroll>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Wear-leveling SPI EEPROM Driver
#define SPI_DRIVER SPID2
#define SPI_SCK_PIN B13
#define SPI_MOSI_PIN B15
#define SPI_MISO_PIN B14

#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN B12
#define EXTERNAL_FLASH_CLOCK_DIVISOR 4
