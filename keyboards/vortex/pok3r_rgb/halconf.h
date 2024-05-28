/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#pragma once

#define HAL_USE_PAL TRUE
#define HAL_USE_USB TRUE

#ifdef EEPROM_DRIVER
#    define HAL_USE_SPI TRUE
#    define SPI_USE_WAIT TRUE
#    define SPI_SELECT_MODE SPI_SELECT_MODE_PAD
#endif

#define SERIAL_USB_BUFFERS_SIZE 256

#include_next <halconf.h>
