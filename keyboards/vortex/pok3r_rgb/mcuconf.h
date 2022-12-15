/* Copyright 2020 QMK
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define HT32F1654_MCUCONF

// Clock configuration

// This configuration:
// 8 MHz HSE crystal
// PLL multiplies HSE to 72 MHz core and peripheral clock
// 72 MHz to UART
// 48 MHz to USB

#define HT32_CK_HSE_FREQUENCY   8000000UL           // 8 MHz
#define HT32_CKCU_SW            CKCU_GCCR_SW_PLL
#define HT32_PLL_USE_HSE        TRUE
#define HT32_PLL_FBDIV          18                  // 8 MHz -> 144 MHz
#define HT32_PLL_OTDIV          0
#define HT32_AHB_PRESCALER      2                   // 144 MHz -> 72 MHz
#define HT32_USART_PRESCALER    1                   // 72 MHz
#define HT32_USB_PRESCALER      3                   // 144 MHz -> 48 MHz
// SysTick uses processor clock at 72MHz
#define HT32_ST_USE_HCLK        TRUE

// Peripheral driver settings

#define HT32_SPI_USE_SPI1                   TRUE
#define HT32_SPI1_IRQ_PRIORITY              6

// USB driver settings

#define HT32_USB_USE_USB0                   TRUE
#define HT32_USB_USB0_IRQ_PRIORITY          5

#ifdef RGB_MATRIX_ENABLE
// PWM
#    define HT32_PWM_USE_GPTM1 TRUE

// GPT
#    define HT32_GPT_USE_BFTM0 TRUE
#endif
