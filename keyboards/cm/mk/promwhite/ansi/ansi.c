/* Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
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
 *    SDI->|       |->SDO1_2
 * R_COL0<-|       |->unused
 * R_COL1<-|       |->unused
 * R_COL2<-|       |->R_COL13
 * R_COL3<-|MBIA043|->R_COL12
 * R_COL4<-|   A   |->R_COL11
 * R_COL5<-|       |->R_COL10
 * R_COL6<-|       |->R_COL9
 * R_COL7<-|_______|->R_COL8
 *          _______
 * SDO1_2->|       |->SDO2_3
 * G_COL0<-|       |->unused
 * G_COL1<-|       |->unused
 * G_COL2<-|       |->G_COL13
 * G_COL3<-|MBIA043|->G_COL12
 * G_COL4<-|   B   |->G_COL11
 * G_COL5<-|       |->G_COL10
 * G_COL6<-|       |->G_COL9
 * G_COL7<-|_______|->G_COL8
 *          _______
 * SDO2_3->|       |->SDO
 * B_COL0<-|       |->unused
 * B_COL1<-|       |->unused
 * B_COL2<-|       |->B_COL13
 * B_COL3<-|MBIA043|->B_COL12
 * B_COL4<-|   C   |->B_COL11
 * B_COL5<-|       |->B_COL10
 * B_COL6<-|       |->B_COL9
 * B_COL7<-|_______|->B_COL8
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
        { MBI_UNUSED_CH, 14 },
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
    /*          Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 Col14 Col15 */
    /*Row0*/ {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,   11,   12,   26,   NLD,  NLD },
    /*Row1*/ {  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,   24,   25,   NLD,  NLD,  NLD },
    /*Row2*/ {  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,   42,   43,   44,   NLD,  NLD },
    /*Row3*/ {  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,   60,   NLD,  61,   NLD,  NLD },
    /*Row4*/ {  65,  NLD, 66,  67,  68,  69,  70,  71,  72,  73,  74,   75,   NLD,  76,   NLD,  NLD },
    /*Row5*/ {  81,  82,  83,  NLD, NLD, NLD, 84,  NLD, NLD, 85,  86,   87,   88,   27,   NLD,  NLD },
    /*Row6*/ {  89,  90,  77,  78,  62,  63,  45,  46,  28,  NLD, NLD,  NLD,  NLD,  NLD,  NLD,  NLD },
    /*Row7*/ {  91,  80,  79,  64,  NLD, 48,  47,  29,  30,  NLD, NLD,  92,   93,   94,   NLD,  NLD },
};
// clang-format on

// Indicator lights for lock keys
bool led_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    // Indicator lights only use blue color channel
    led_matrix_set_value(92, host_keyboard_led_state().num_lock ? 0xff : 0x0);
    led_matrix_set_value(93, host_keyboard_led_state().caps_lock ? 0xff : 0x0);
    led_matrix_set_value(94, host_keyboard_led_state().scroll_lock ? 0xff : 0x0);
    return led_matrix_indicators_advanced_user(led_min, led_max);
}
#endif
