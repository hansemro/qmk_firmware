// Copyright 2022 Hansem Ro (@hansemro)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// CK_HSE = 8 MHz crystal
// CK_PLL = CK_HSE * 18 = 144 MHz
// CK_SYS = CK_PLL
// CK_AHB = CK_SYS / 2 = 72 MHz
// CK_USB = CK_PLL / 3 = 48 MHz

#include_next <mcuconf.h>

#ifdef EEPROM_DRIVER
// SPI
#    undef HT32_SPI_USE_SPI1
#    define HT32_SPI_USE_SPI1 TRUE
#endif

#ifdef RGB_MATRIX_ENABLE
// PWM
#    undef HT32_PWM_USE_GPTM1
#    define HT32_PWM_USE_GPTM1 TRUE

// GPT
#    undef HT32_GPT_USE_BFTM0
#    define HT32_GPT_USE_BFTM0 TRUE
#endif
