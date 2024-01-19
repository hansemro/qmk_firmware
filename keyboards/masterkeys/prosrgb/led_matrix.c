#include "config.h"
#include "mbi.h"

#define NLD 255

// clang-format off
const mbi_channel_t mbi_channels[MBI_NUM_CASCADE][MBI_NUM_CHANNELS] = {
    {
        {MBI_RED_CH, 0},
        {MBI_RED_CH, 1},
        {MBI_RED_CH, 2},
        {MBI_RED_CH, 3},
        {MBI_RED_CH, 4},
        {MBI_RED_CH, 5},
        {MBI_RED_CH, 6},
        {MBI_RED_CH, 7},
        {MBI_RED_CH, 8},
        {MBI_UNUSED_CH, 9},
        {MBI_UNUSED_CH, 10},
        {MBI_RED_CH, 11},
        {MBI_RED_CH, 12},
        {MBI_RED_CH, 13},
        {MBI_RED_CH, 14},
        {MBI_UNUSED_CH, 15},
    },
    {
        {MBI_GREEN_CH, 0},
        {MBI_GREEN_CH, 1},
        {MBI_GREEN_CH, 2},
        {MBI_GREEN_CH, 3},
        {MBI_GREEN_CH, 4},
        {MBI_GREEN_CH, 5},
        {MBI_GREEN_CH, 6},
        {MBI_GREEN_CH, 7},
        {MBI_GREEN_CH, 8},
        {MBI_UNUSED_CH, 9},
        {MBI_UNUSED_CH, 10},
        {MBI_GREEN_CH, 11},
        {MBI_GREEN_CH, 12},
        {MBI_GREEN_CH, 13},
        {MBI_GREEN_CH, 14},
        {MBI_UNUSED_CH, 15},
    },
    {
        {MBI_BLUE_CH, 0},
        {MBI_BLUE_CH, 1},
        {MBI_BLUE_CH, 2},
        {MBI_BLUE_CH, 3},
        {MBI_BLUE_CH, 4},
        {MBI_BLUE_CH, 5},
        {MBI_BLUE_CH, 6},
        {MBI_BLUE_CH, 7},
        {MBI_BLUE_CH, 8},
        {MBI_UNUSED_CH, 9},
        {MBI_UNUSED_CH, 10},
        {MBI_BLUE_CH, 11},
        {MBI_BLUE_CH, 12},
        {MBI_BLUE_CH, 13},
        {MBI_BLUE_CH, 14},
        {MBI_UNUSED_CH, 15},
    }
};

// LED Matrix to LED Index
const uint8_t led_matrix_co[MATRIX_ROWS][MATRIX_COLS] = {
    /*           Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 Col14 */
    /* Row0 */ { 16,  18,  20,  22,  24,  26,  28,  30,  32,  NLD,  NLD, NLD,  0,    4,    7,   },
    /* Row1 */ { 17,  19,  21,  23,  25,  27,  29,  31,  NLD, NLD,  NLD, NLD,  1,    5,    8,   },
    /* Row2 */ { 33,  35,  37,  39,  41,  43,  45,  47,  49,  NLD,  NLD, 77,   2,    6,    9,   },
    /* Row3 */ { 34,  36,  38,  40,  42,  44,  46,  48,  10,  NLD,  NLD, 76,   3,    15,   NLD, },
    /* Row4 */ { 50,  52,  54,  56,  58,  60,  62,  82,  11,  NLD,  NLD, 78,   NLD,  NLD,  NLD, },
    /* Row5 */ { 51,  53,  55,  57,  59,  61,  NLD, 75,  12,  NLD,  NLD, 79,   NLD,  NLD,  NLD, },
    /* Row6 */ { 63,  65,  67,  69,  71,  73,  83,  NLD, 86,  NLD,  NLD, 80,   13,   NLD,  NLD, },
    /* Row7 */ { 64,  66,  68,  70,  72,  74,  85,  84,  NLD, NLD,  NLD, 81,   14,   NLD,  NLD, }
};
// clang-format on
