/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

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

#define HT32F523x2_MCUCONF

/*
 * HT32F523xx drivers configuration.
 * The following settings override the default settings present in
 * the various device driver implementation headers.
 * Note that the settings for each driver only have effect if the whole
 * driver is enabled in halconf.h.
 *
 * IRQ priorities:
 * 15...0       Lowest...Highest.
 */

/*
 * HAL driver system settings.
 */
#define HT32_CK_HSE_FREQUENCY               8000000UL // 8 MHz
#define HT32_CKCU_SW                        CKCU_GCCR_SW_PLL
#define HT32_PLL_USE_HSE                    TRUE
#define HT32_PLL_FBDIV                      6 // 8 MHz -> 48 MHz
#define HT32_PLL_OTDIV                      0
#define HT32_AHB_PRESCALER                  1 // 48 MHz -> 48 MHz
#define HT32_USB_PRESCALER                  1 // 48 MHz -> 48 MHz

/*
 * ST driver system settings.
 */
#define HT32_ST_USE_HCLK                    TRUE
#define HT32_ST_IRQ_PRIORITY                8

/*
 * GPT driver system settings.
 */
#define HT32_GPT_USE_BFTM0                  FALSE
#define HT32_GPT_USE_BFTM1                  FALSE
#define HT32_GPT_BFTM0_IRQ_PRIORITY         7
#define HT32_GPT_BFTM1_IRQ_PRIORITY         7

/*
 * PWM driver system settings.
 */
#define HT32_PWM_USE_MCTM0                  FALSE
#define HT32_PWM_USE_MCTM1                  FALSE
#define HT32_PWM_USE_GPTM0                  FALSE
#define HT32_PWM_USE_GPTM1                  FALSE

/*
 * SPI driver system settings.
 */
#define HT32_SPI_USE_SPI0                   FALSE
#define HT32_SPI_USE_SPI1                   FALSE
#define HT32_SPI0_IRQ_PRIORITY              9
#define HT32_SPI1_IRQ_PRIORITY              9

/*
 * SERIAL driver system settings.
 */
#define HT32_SERIAL_USE_USART0              FALSE
#define HT32_SERIAL_USE_USART1              FALSE
#define HT32_USART0_IRQ_PRIORITY            12
#define HT32_USART1_IRQ_PRIORITY            12

/*
 * UART driver system settings.
 */
#define HT32_UART_USE_UART0                 FALSE
#define HT32_UART_USE_UART1                 FALSE
#define HT32_UART_USE_USART0                FALSE
#define HT32_UART_USE_USART1                FALSE
#define HT32_UART0_IRQ_PRIORITY             12
#define HT32_UART1_IRQ_PRIORITY             12
#define HT32_USART0_IRQ_PRIORITY            12
#define HT32_USART1_IRQ_PRIORITY            12

/*
 * USB driver system settings.
 */
#define HT32_USB_USE_USB0                   TRUE
#define HT32_USB_USB0_IRQ_PRIORITY          5
