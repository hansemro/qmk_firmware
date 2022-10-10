// Copyright 2022 Hansem Ro (@hansemro)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "pin_defs.h"

#ifdef RGB_MATRIX_ENABLE
#define DRIVER_LED_TOTAL 87
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES
#endif

// Bootmagic key on Esc
#define BOOTMAGIC_LITE_COLUMN 12
#define BOOTMAGIC_LITE_ROW 0
