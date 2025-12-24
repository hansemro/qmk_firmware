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

#ifdef RGB_MATRIX_ENABLE
#    include "host.h"
#    include "rgb_matrix.h"
#    include "mbi.h"

/*
 * Channel Setup:
 *          _______
 *    SDI->|       |->SDO1_2
 * R_COL0<-|       |->unused
 * R_COL1<-|       |->R_COL14
 * R_COL2<-|       |->R_COL13
 * R_COL3<-|MBIA043|->R_COL12
 * R_COL4<-|   A   |->R_COL11
 * R_COL5<-|       |->R_COL10
 * R_COL6<-|       |->unused
 * R_COL7<-|_______|->unused
 *          _______
 * SDO1_2->|       |->SDO2_3
 * G_COL0<-|       |->unused
 * G_COL1<-|       |->G_COL14
 * G_COL2<-|       |->G_COL13
 * G_COL3<-|MBIA043|->G_COL12
 * G_COL4<-|   B   |->G_COL11
 * G_COL5<-|       |->G_COL10
 * G_COL6<-|       |->unused
 * G_COL7<-|_______|->unused
 *          _______
 * SDO2_3->|       |->SDO
 * B_COL0<-|       |->unused
 * B_COL1<-|       |->B_COL14
 * B_COL2<-|       |->B_COL13
 * B_COL3<-|MBIA043|->B_COL12
 * B_COL4<-|   C   |->B_COL11
 * B_COL5<-|       |->B_COL10
 * B_COL6<-|       |->unused
 * B_COL7<-|_______|->unused
 */
// clang-format off
const mbi_channel_t g_mbi_channels[MBI_NUM_DRIVER][MBI_NUM_CHANNELS] = {
    {
        { MBI_RED_CH, 0 },
        { MBI_RED_CH, 1 },
        { MBI_RED_CH, 2 },
        { MBI_RED_CH, 3 },
        { MBI_RED_CH, 4 },
        { MBI_RED_CH, 5 },
        { MBI_RED_CH, 6 },
        { MBI_RED_CH, 7 },
        { MBI_RED_CH, 8 },
        { MBI_UNUSED_CH, 9 },
        { MBI_UNUSED_CH, 10 },
        { MBI_RED_CH, 11 },
        { MBI_RED_CH, 12 },
        { MBI_RED_CH, 13 },
        { MBI_RED_CH, 14 },
        { MBI_UNUSED_CH, 15 },
    },
    {
        { MBI_GREEN_CH, 0 },
        { MBI_GREEN_CH, 1 },
        { MBI_GREEN_CH, 2 },
        { MBI_GREEN_CH, 3 },
        { MBI_GREEN_CH, 4 },
        { MBI_GREEN_CH, 5 },
        { MBI_GREEN_CH, 6 },
        { MBI_GREEN_CH, 7 },
        { MBI_GREEN_CH, 8 },
        { MBI_UNUSED_CH, 9 },
        { MBI_UNUSED_CH, 10 },
        { MBI_GREEN_CH, 11 },
        { MBI_GREEN_CH, 12 },
        { MBI_GREEN_CH, 13 },
        { MBI_GREEN_CH, 14 },
        { MBI_UNUSED_CH, 15 },
    },
    {
        { MBI_BLUE_CH, 0 },
        { MBI_BLUE_CH, 1 },
        { MBI_BLUE_CH, 2 },
        { MBI_BLUE_CH, 3 },
        { MBI_BLUE_CH, 4 },
        { MBI_BLUE_CH, 5 },
        { MBI_BLUE_CH, 6 },
        { MBI_BLUE_CH, 7 },
        { MBI_BLUE_CH, 8 },
        { MBI_UNUSED_CH, 9 },
        { MBI_UNUSED_CH, 10 },
        { MBI_BLUE_CH, 11 },
        { MBI_BLUE_CH, 12 },
        { MBI_BLUE_CH, 13 },
        { MBI_BLUE_CH, 14 },
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
const uint8_t g_mbi_led_matrix_co[MATRIX_ROWS][MBI_NUM_CHANNELS] = {
    /*           Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 Col14 Col15*/
    /* Row0 */ { 16,  18,  20,  22,  24,  26,  28,  30,  32,  NLD,  NLD, NLD,  0,    4,    7,    NLD },
    /* Row1 */ { 17,  19,  21,  23,  25,  27,  29,  31,  NLD, NLD,  NLD, 61,   1,    5,    8,    NLD },
    /* Row2 */ { 33,  35,  37,  39,  41,  43,  45,  46,  48,  NLD,  NLD, 78,   2,    6,    9,    NLD },
    /* Row3 */ { 34,  36,  38,  40,  42,  44,  NLD, 47,  10,  NLD,  NLD, 77,   3,    15,   NLD,  NLD },
    /* Row4 */ { 49,  51,  53,  55,  57,  59,  62,  83,  11,  NLD,  NLD, 79,   64,  NLD,  NLD,  NLD },
    /* Row5 */ { 50,  52,  54,  56,  58,  60,  NLD, 76,  12,  NLD,  NLD, 80,   NLD,  NLD,  NLD,  NLD },
    /* Row6 */ { 63,  66,  68,  70,  72,  74,  84,  NLD, 87,  NLD,  NLD, 81,   13,   NLD,  NLD,  NLD },
    /* Row7 */ { 65,  67,  69,  71,  73,  75,  86,  85,  NLD, NLD,  NLD, 82,   14,   NLD,  NLD,  NLD },
};
// clang-format on

// Indicator lights for lock keys
bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    if (!host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(49, 0x0, 0x0, 0x0);
    }
    if (!host_keyboard_led_state().scroll_lock) {
        rgb_matrix_set_color(14, 0x0, 0x0, 0x0);
    }
    return rgb_matrix_indicators_advanced_user(led_min, led_max);
}
#endif
