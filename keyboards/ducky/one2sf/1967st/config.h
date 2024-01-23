/*
Copyright 2019 /u/KeepItUnder
Copyright 2023 Hayley Hughes

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define DIP_SWITCH_MATRIX_GRID { {0,14}, {1,14}, {2,14}, {3,14} }

#define GPIO_INPUT_PIN_DELAY (NUC123_HCLK / 6 / 1000000L)

#define ENABLE_RGB_MATRIX_BREATHING
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_TYPING_HEATMAP

// MBIA045
#define MBI_NUM_CHANNELS 16
#define MBI_SHIFT_REG_WIDTH 16
#define MBI_DATA_LATCH 1
#define MBI_GLOBAL_LATCH 3
#define MBI_WRITE_CONFIGURATION 11
#define MBI_ENABLE_WRITE_CONFIGURATION 15

// MBI configuration
#define MBI_CONFIGURATION 0x8430

#define MBI_NUM_DRIVER 3
#define MBI_LOAD_TYPE MBI_LOAD_TYPE_PARALLEL
// Insert no-ops to meet timing requirements
#define MBI_NOPS 2

// PWM to generate GCLK clock signal
// Desired output/GCLK frequency =  2.0 MHz
#define MBI_PWM_DRIVER PWMD1
#define MBI_PWM_CHANNEL 0
#define MBI_PWM_OUTPUT_MODE PWM_OUTPUT_ACTIVE_LOW
#define MBI_PWM_COUNTER_FREQUENCY (2000000UL * 2)
#define MBI_PWM_PERIOD 2UL

// MBI timer for flushing color data for a single row
#define MBI_TIMER_DRIVER GPTD0

// MCU manages row pins; MBI manages column pins
#define MBI_LED_DIRECTION ROW2COL
// MCU-managed LED row pins
#define MBI_LED_GPIO_PINS \
    { C4, C5, B3, B2, D9 }
#define MBI_LED_GPIO_ACTIVE_HL HIGH
#define MBI_NUM_LED_GPIO_PINS 5
#define MBI_LED_GPIO_OUTPUT_MODE PAL_MODE_OUTPUT_PUSHPULL

// MCU-managed MBI pins
#define MBI_LE_PIN D3
#define MBI_LE_OUTPUT_MODE PAL_MODE_OUTPUT_PUSHPULL
#define MBI_SDI_PINS \
    { B14, B13, B12 }
#define MBI_SDI_OUTPUT_MODE PAL_MODE_OUTPUT_PUSHPULL
#define MBI_DCLK_PIN D4
#define MBI_DCLK_OUTPUT_MODE PAL_MODE_OUTPUT_PUSHPULL
#define MBI_GCLK_PIN A12
#define MBI_GCLK_OUTPUT_MODE PAL_MODE_OUTPUT_PUSHPULL

// MCU-managed MBI power supply enable (active-low)
#define MBI_POWER_ENABLE_PIN D5
#define MBI_POWER_ACTIVE_HL LOW
#define MBI_POWER_OUTPUT_MODE PAL_MODE_OUTPUT_PUSHPULL
