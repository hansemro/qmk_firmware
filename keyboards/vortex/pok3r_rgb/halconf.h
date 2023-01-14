/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#pragma once

#define HAL_USE_PAL TRUE
#define HAL_USE_USB TRUE
#define HAL_USE_WDG TRUE

#ifdef RGB_MATRIX_ENABLE
#    define HAL_USE_GPT TRUE
#    define HAL_USE_PWM TRUE
#endif

#ifdef EEPROM_DRIVER
#    define HAL_USE_SPI TRUE
#    define SPI_USE_WAIT TRUE
#    define SPI_SELECT_MODE SPI_SELECT_MODE_PAD
#endif

#define SERIAL_USB_BUFFERS_SIZE 256

#include_next <halconf.h>
