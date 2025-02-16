// Copyright 2025 HorrorTroll <https://github.com/HorrorTroll>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <board.h>

#undef AT32F415KB
#define AT32F415RB

#undef AT32_HEXTCLK
#define AT32_HEXTCLK 16000000
