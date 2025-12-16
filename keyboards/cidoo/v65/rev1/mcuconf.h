// Copyright 2025 HorrorTroll <https://github.com/HorrorTroll>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <mcuconf.h>

#undef AT32_GPT_USE_TMR3
#define AT32_GPT_USE_TMR3 TRUE

#undef AT32_PWM_USE_TMR1
#define AT32_PWM_USE_TMR1 TRUE

#undef AT32_SPI_USE_SPI2
#define AT32_SPI_USE_SPI2 TRUE

#undef AT32_PLLCFGEN
#define AT32_PLLCFGEN AT32_PLLCFGEN_FLEX

#undef AT32_PLL_NS_VALUE
#define AT32_PLL_NS_VALUE 36
