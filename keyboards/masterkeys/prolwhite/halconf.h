/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */
#pragma once

#define HAL_USE_PAL TRUE
#define HAL_USE_USB TRUE

#define SERIAL_USB_BUFFERS_SIZE 256

#include_next <halconf.h>
