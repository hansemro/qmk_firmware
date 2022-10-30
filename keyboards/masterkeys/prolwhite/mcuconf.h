/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio
    Copyright (C) 2022 Hansem Ro <hansemro@outlook.com>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#pragma once

#include "nvic.h"

#define HT32F52352_MCUCONF

// Internal 8 MHz RC oscillator clock (HSI)
// CK_PLL = 8 MHz HSI * 6 = 48 MHz
// CK_SYS = CK_PLL
// CK_AHB = CK_SYS / 1 = 48 MHz
// CK_USB = CK_PLL = 48 MHz

#define HT32_CKCU_SW CKCU_GCCR_SW_PLL
#define HT32_PLL_USE_HSE FALSE
#define HT32_PLL_FBDIV 6 // 8 MHz -> 48 MHz
#define HT32_PLL_OTDIV 0
#define HT32_AHB_PRESCALER 1   // 48 MHz -> 48 MHz
#define HT32_USB_PRESCALER 1   // 48 MHz -> 48 MHz
// SysTick uses processor clock at 48MHz
#define HT32_ST_USE_HCLK TRUE

#define HT32_USB_USE_USB0 TRUE
#define HT32_USB_USB0_IRQ_PRIORITY 5

#ifdef LED_MATRIX_ENABLE
#    define HT32_PWM_USE_GPTM1 TRUE
#    define HT32_GPT_USE_BFTM1 TRUE
#endif
