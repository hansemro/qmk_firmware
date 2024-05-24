/* Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
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

#ifdef RGB_MATRIX_ENABLE
#    include "mbi.h"
#    include "rgb_matrix.h"
#    include "host.h"

/*
 * Channel Setup:
 *         ┌───────┐
 *    SDI->│       │->SDO1_2
 * R_ROW0<-│       │->G_ROW6
 * R_ROW1<-│       │->G_ROW5
 * R_ROW2<-│       │->G_ROW4
 * R_ROW3<-│MBIA043│->G_ROW3
 * R_ROW4<-│   A   │->G_ROW2
 * R_ROW5<-│       │->G_ROW1
 * R_ROW6<-│       │->G_ROW0
 * R_ROW7<-│       │->R_ROW8
 *         └───────┘
 *         ┌───────┐
 * SDO1_2->│       │->SDO
 * G_ROW7<-│       │->unused
 * G_ROW8<-│       │->unused
 * B_ROW0<-│       │->unused
 * B_ROW1<-│MBIA043│->unused
 * B_ROW2<-│   B   │->unused
 * B_ROW3<-│       │->B_ROW8
 * B_ROW4<-│       │->B_ROW7
 * B_ROW5<-│       │->B_ROW6
 *         └───────┘
 */
// clang-format off
const mbi_channel_t g_mbi_channels[MBI_NUM_DRIVER][MBI_NUM_CHANNELS] = {
    /* MBI A */
    {
    /*    Color channel (RED, GREEN, BLUE, or UNUSED)
     *    |
     *    |           MBI-managed row index
     *    |           | */
        { MBI_RED_CH, 0 }, /* MBI output channel 0 */
        { MBI_RED_CH, 1 }, /* MBI output channel 1 */
        { MBI_RED_CH, 2 }, /* ... */
        { MBI_RED_CH, 3 },
        { MBI_RED_CH, 4 },
        { MBI_RED_CH, 5 },
        { MBI_RED_CH, 6 },
        { MBI_RED_CH, 7 },
        { MBI_RED_CH, 8 },
        { MBI_GREEN_CH, 0 },
        { MBI_GREEN_CH, 1 },
        { MBI_GREEN_CH, 2 },
        { MBI_GREEN_CH, 3 },
        { MBI_GREEN_CH, 4 },
        { MBI_GREEN_CH, 5 },
        { MBI_GREEN_CH, 6 }, /* MBI output channel 15 */
    },
    /* MBI B */
    {
        { MBI_GREEN_CH, 7 },
        { MBI_GREEN_CH, 8 },
        { MBI_BLUE_CH, 0 },
        { MBI_BLUE_CH, 1 },
        { MBI_BLUE_CH, 2 },
        { MBI_BLUE_CH, 3 },
        { MBI_BLUE_CH, 4 },
        { MBI_BLUE_CH, 5 },
        { MBI_BLUE_CH, 6 },
        { MBI_BLUE_CH, 7 },
        { MBI_BLUE_CH, 8 },
        { MBI_UNUSED_CH, 0 },
        { MBI_UNUSED_CH, 0 },
        { MBI_UNUSED_CH, 0 },
        { MBI_UNUSED_CH, 0 },
        { MBI_UNUSED_CH, 0 },
    },
};
// clang-format on

#    ifndef NO_LED
#        define NLD 255
#    else
#        define NLD NO_LED
#    endif

// LED Matrix to LED Index
// Since COL2ROW, MBI manages rows while MCU manages columns
// clang-format off
const uint8_t g_mbi_led_matrix_co[MBI_NUM_CHANNELS][MBI_NUM_LED_GPIO_PINS] = {
    /*          Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7*/
    /*Row0*/ {  0,   1,   2,   3,   4,   5,   6,   7    },
    /*Row1*/ {  14,  15,  16,  17,  18,  19,  20,  21   },
    /*Row2*/ {  28,  29,  30,  31,  32,  33,  34,  35   },
    /*Row3*/ {  42,  44,  45,  46,  47,  48,  49,  50   },
    /*Row4*/ {  55,  56,  57,  8,   9,   10,  11,  13   },
#if defined(LAYOUT_60_iso)
    /*Row5*/ {  43,  65,  12,  22,  23,  24,  25,  40   },
#else // Not currently possible to map (5,7) to different LED positions...
    /*Row5*/ {  43,  65,  12,  22,  23,  24,  25,  27   },
#endif
    /*Row6*/ {  NLD, NLD, 26,  36,  37,  38,  39,  41   },
    /*Row7*/ {  NLD, 58,  59,  51,  52,  53,  54,  NLD  },
    /*Row8*/ {  NLD, NLD, 60,  61,  62,  63,  NLD, 64   },
    /*Row9*/ {  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
    /*Row10*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
    /*Row11*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
    /*Row12*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
    /*Row13*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
    /*Row14*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
    /*Row15*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
};
// clang-format on

// Caps lock indicator light
bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    uint8_t val = host_keyboard_led_state().caps_lock ? 0xff : 0x0;
    rgb_matrix_set_color(65, val, val, val);
    return rgb_matrix_indicators_advanced_user(led_min, led_max);
}

#endif
