/* Copyright (c) 2025 Hansem Ro <hansemro@outlook.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef LED_MATRIX_ENABLE
#    include "host.h"
#    include "led_matrix.h"
#    include "mbi.h"

/*
 * Channel Setup:
 *          _______
 *    SDI->|       |->SDO
 *   COL0<-|       |->unused
 *   COL1<-|       |->COL14
 *   COL2<-|       |->COL13
 *   COL3<-|MBIA043|->COL12
 *   COL4<-|   A   |->COL11
 *   COL5<-|       |->COL10
 *   COL6<-|       |->COL9
 *   COL7<-|_______|->COL8
 */
// clang-format off
const mbi_channel_t g_mbi_channels[MBI_NUM_DRIVER][MBI_NUM_CHANNELS] = {
    {
        { MBI_MONO_CH, 0 },
        { MBI_MONO_CH, 1 },
        { MBI_MONO_CH, 2 },
        { MBI_MONO_CH, 3 },
        { MBI_MONO_CH, 4 },
        { MBI_MONO_CH, 5 },
        { MBI_MONO_CH, 6 },
        { MBI_MONO_CH, 7 },
        { MBI_MONO_CH, 8 },
        { MBI_MONO_CH, 9 },
        { MBI_MONO_CH, 10 },
        { MBI_MONO_CH, 11 },
        { MBI_MONO_CH, 12 },
        { MBI_MONO_CH, 13 },
        { MBI_MONO_CH, 14 },
        { MBI_UNUSED_CH, 15 },
    },
};
// clang-format on

#ifndef NLD
#    ifdef NO_LED
#        define NLD NO_LED
#    else
#        define NLD 255
#    endif
#else
#    error "NLD already defined"
#endif

// LED Matrix to LED Index
// clang-format off
const uint8_t g_mbi_led_matrix_co[MBI_NUM_LED_GPIO_PINS][MBI_NUM_CHANNELS] = {
    /*           Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 Col14 Col15*/
    /* Row0 */ { 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,   11,   12,   13,   14,   NLD },
    /* Row1 */ { 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,   27,   28,   NLD,  29,   NLD },
    /* Row2 */ { 33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,   44,   45,   NLD,  30,   NLD },
    /* Row3 */ { 49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,   60,   61,   62,   46,   NLD },
    /* Row4 */ { 63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,   74,   NLD,  75,   76,   NLD },
    /* Row5 */ { 77,  78,  79,  NLD, NLD, NLD, 80,  NLD, NLD, 81,  82,   83,   84,   85,   86,   NLD },
    /* Row6 */ { NLD, 15,  31,  32,  NLD, NLD, NLD, NLD, 47,  48,  NLD,  NLD,  NLD,  NLD,  87,   NLD },
};
// clang-format on

// Indicator lights for lock keys
bool led_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    led_matrix_set_value(49, host_keyboard_led_state().caps_lock ? 0xff : 0x0);
    led_matrix_set_value(14, host_keyboard_led_state().scroll_lock ? 0xff : 0x0);
    return led_matrix_indicators_advanced_user(led_min, led_max);
}
#endif
