// Copyright 2022 Hansem Ro (@hansemro)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "nvic.h"

#define HT32F1654_MCUCONF

// CK_HSE = 8 MHz crystal
// CK_PLL = 144 MHz
// CK_SYS = CK_PLL
// CK_AHB = CK_SYS / 2 = 72 MHz
// CK_USB = CK_PLL / 3 = 48 MHz

#define HT32_CK_HSE_FREQUENCY 8000000UL // 8 MHz
#define HT32_CKCU_SW CKCU_GCCR_SW_PLL
#define HT32_PLL_USE_HSE TRUE
#define HT32_PLL_FBDIV 18 // 8 MHz -> 144 MHz
#define HT32_PLL_OTDIV 0
#define HT32_AHB_PRESCALER 2 // 144 MHz -> 72 MHz
#define HT32_USB_PRESCALER 3 // 144 MHz -> 48 MHz
// SysTick uses processor clock at 72MHz
#define HT32_ST_USE_HCLK TRUE

#define HT32_USB_USE_USB0 TRUE
#define HT32_USB_USB0_IRQ_PRIORITY 5

//#define HT32_SPI_USE_SPI1 TRUE
//#define HT32_SPI1_IRQ_PRIORITY 9

#ifdef RGB_MATRIX_ENABLE
// PWM
#define HT32_PWM_USE_GPTM1 TRUE

// GPT
#define HT32_GPT_USE_BFTM1 TRUE
#endif
