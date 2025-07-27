// Copyright 2025 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Internal 8 MHz RC oscillator clock (HSI)
// CK_PLL = 8 MHz HSI * 6 = 48 MHz
// CK_SYS = CK_PLL
// CK_AHB = CK_SYS / 1 = 48 MHz
// CK_USB = CK_PLL = 48 MHz

#include_next "mcuconf.h"

// Use HSI
#undef HT32_PLL_USE_HSE
#define HT32_PLL_USE_HSE FALSE


#ifdef EEPROM_DRIVER
// SPI
#    undef HT32_SPI_USE_SPI1
#    define HT32_SPI_USE_SPI1 TRUE
#endif
