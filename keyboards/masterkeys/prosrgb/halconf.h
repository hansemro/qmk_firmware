// Copyright 2022 Hansem Ro (@hansemro)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define HAL_USE_PAL     TRUE
#define HAL_USE_USB     TRUE
//#define HAL_USE_GPT     TRUE

//#define HAL_USE_SPI     TRUE
//#define SPI_USE_WAIT    TRUE
//#define SPI_SELECT_MODE SPI_SELECT_MODE_PAD

#define SERIAL_USB_BUFFERS_SIZE 256

#include_next <halconf.h>
