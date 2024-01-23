#ifdef RGB_MATRIX_ENABLE
#include "mbi.h"

// clang-format off
const mbi_channel_t g_mbi_channels[MBI_NUM_DRIVER][MBI_NUM_CHANNELS] = {
    { /* B14 */
        {MBI_RED_CH, 0},
        {MBI_RED_CH, 1},
        {MBI_RED_CH, 2},
        {MBI_RED_CH, 3},
        {MBI_RED_CH, 4},
        {MBI_RED_CH, 5},
        {MBI_RED_CH, 6},
        {MBI_RED_CH, 7},
        {MBI_RED_CH, 8},
        {MBI_RED_CH, 9},
        {MBI_RED_CH, 10},
        {MBI_RED_CH, 11},
        {MBI_RED_CH, 12},
        {MBI_RED_CH, 13},
        {MBI_RED_CH, 14},
        {MBI_UNUSED_CH, 0},
    },
    { /* B13 */
        {MBI_GREEN_CH, 0},
        {MBI_GREEN_CH, 1},
        {MBI_GREEN_CH, 2},
        {MBI_GREEN_CH, 3},
        {MBI_GREEN_CH, 4},
        {MBI_GREEN_CH, 5},
        {MBI_GREEN_CH, 6},
        {MBI_GREEN_CH, 7},
        {MBI_GREEN_CH, 8},
        {MBI_GREEN_CH, 9},
        {MBI_GREEN_CH, 10},
        {MBI_GREEN_CH, 11},
        {MBI_GREEN_CH, 12},
        {MBI_GREEN_CH, 13},
        {MBI_GREEN_CH, 14},
        {MBI_UNUSED_CH, 0},
    },
    { /* B12 */
        {MBI_BLUE_CH, 0},
        {MBI_BLUE_CH, 1},
        {MBI_BLUE_CH, 2},
        {MBI_BLUE_CH, 3},
        {MBI_BLUE_CH, 4},
        {MBI_BLUE_CH, 5},
        {MBI_BLUE_CH, 6},
        {MBI_BLUE_CH, 7},
        {MBI_BLUE_CH, 8},
        {MBI_BLUE_CH, 9},
        {MBI_BLUE_CH, 10},
        {MBI_BLUE_CH, 11},
        {MBI_BLUE_CH, 12},
        {MBI_BLUE_CH, 13},
        {MBI_BLUE_CH, 14},
        {MBI_UNUSED_CH, 0},
    },
};
// clang-format on

#ifndef NO_LED
#    define NLD 255
#else
#    define NLD NO_LED
#endif

//const mbi_led g_mbi_leds[RGB_MATRIX_LED_COUNT] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9}, {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {1, 13}, {1, 14}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7}, {2, 8}, {2, 9}, {2, 10}, {2, 11}, {2, 13}, {2, 14}, {3, 0}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8}, {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 13}, {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 5}, {4, 7}, {4, 9}, {4, 10}, {4, 11}, {4, 12}, {4, 13}, {4, 14}};

// clang-format off
const uint8_t g_mbi_led_matrix_co[MBI_NUM_LED_GPIO_PINS][MBI_NUM_CHANNELS] = {
    /*           Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 Col14 Col15 */
    /* Row0 */ { 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,    10,  11,   12,   13,   14,  NLD, },
    /* Row1 */ { 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,   25,  26,   27,   28,   29,  NLD, },
    /* Row2 */ { 30,  31,  32,  33,  34,  35,  36,  37,  38,  39,   40,  41,   42,   43,   44,  NLD, },
    /* Row3 */ { 45,  NLD, 46,  47,  48,  49,  50,  51,  52,  53,   54,  55,   56,   57,   NLD, NLD, },
    /* Row4 */ { 58,  59,  60,  61,  NLD, 62,  NLD, 63,  NLD, 64,   65,  66,   67,   68,   69,  NLD, },
};
// clang-format on
#endif
