/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#pragma once

#include <stdint.h>

/* FUNCTIONS */
#ifdef RGB_MATRIX_ENABLE
void mbia043_init(void);
void mbia043_set_mask(int index, uint8_t value);
#endif
