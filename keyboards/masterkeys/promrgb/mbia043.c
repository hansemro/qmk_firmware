/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2023 Hansem Ro <hansemro@outlook.com>
 */

#include "hal.h"
#include "gpio.h"
#include "mbia043.h"
#include "quantum.h"

#ifdef RGB_MATRIX_ENABLE
#    define NL NO_LED

// clang-format off
// LED Matrix to LED Index
uint8_t led_matrix_co[MATRIX_ROWS][MATRIX_COLS] = {
    /*          Col0 Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 */
    /*Row0*/ {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,   11,   12,   26 },
    /*Row1*/ {  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,   24,   25,   NL },
    /*Row2*/ {  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,   42,   43,   44 },
    /*Row3*/ {  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,   60,   NL,   61 },
    /*Row4*/ {  65,  NL,  66,  67,  68,  69,  70,  71,  72,  73,  74,   75,   NL,   76 },
    /*Row5*/ {  81,  82,  83,  NL,  NL,  NL,  84,  NL,  NL,  85,  86,   87,   88,   27 },
    /*Row6*/ {  89,  90,  77,  78,  62,  63,  45,  46,  28,  NL,  NL,   NL,   NL,   NL },
    /*Row7*/ {  91,  80,  79,  64,  NL,  48,  47,  29,  30,  NL,  NL,   92,   93,   94 },
};
// clang-format on

// g_led_config generated from info.json

typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t mask;
} mbia043_led_t;

static mbia043_led_t mbia043_leds[RGB_MATRIX_LED_COUNT];

#    define MBIA043_RED_CH 0
#    define MBIA043_GREEN_CH 1
#    define MBIA043_BLUE_CH 2
#    define MBIA043_UNUSED_CH 3

typedef struct PACKED {
    uint8_t color_ch;
    uint8_t color_index;
} mbia043_channel_t;

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
static const mbia043_channel_t mbia043_channels[MBIA043_NUM_CASCADE][MBIA043_NUM_CHANNELS] = {
    {
        {MBIA043_RED_CH, 0},
        {MBIA043_RED_CH, 1},
        {MBIA043_RED_CH, 2},
        {MBIA043_RED_CH, 3},
        {MBIA043_RED_CH, 4},
        {MBIA043_RED_CH, 5},
        {MBIA043_RED_CH, 6},
        {MBIA043_RED_CH, 7},
        {MBIA043_RED_CH, 8},
        {MBIA043_RED_CH, 9},
        {MBIA043_RED_CH, 10},
        {MBIA043_RED_CH, 11},
        {MBIA043_RED_CH, 12},
        {MBIA043_RED_CH, 13},
        {MBIA043_UNUSED_CH, 14},
        {MBIA043_UNUSED_CH, 15},
    },
    {
        {MBIA043_GREEN_CH, 0},
        {MBIA043_GREEN_CH, 1},
        {MBIA043_GREEN_CH, 2},
        {MBIA043_GREEN_CH, 3},
        {MBIA043_GREEN_CH, 4},
        {MBIA043_GREEN_CH, 5},
        {MBIA043_GREEN_CH, 6},
        {MBIA043_GREEN_CH, 7},
        {MBIA043_GREEN_CH, 8},
        {MBIA043_GREEN_CH, 9},
        {MBIA043_GREEN_CH, 10},
        {MBIA043_GREEN_CH, 11},
        {MBIA043_GREEN_CH, 12},
        {MBIA043_GREEN_CH, 13},
        {MBIA043_UNUSED_CH, 14},
        {MBIA043_UNUSED_CH, 15},
    },
    {
        {MBIA043_BLUE_CH, 0},
        {MBIA043_BLUE_CH, 1},
        {MBIA043_BLUE_CH, 2},
        {MBIA043_BLUE_CH, 3},
        {MBIA043_BLUE_CH, 4},
        {MBIA043_BLUE_CH, 5},
        {MBIA043_BLUE_CH, 6},
        {MBIA043_BLUE_CH, 7},
        {MBIA043_BLUE_CH, 8},
        {MBIA043_BLUE_CH, 9},
        {MBIA043_BLUE_CH, 10},
        {MBIA043_BLUE_CH, 11},
        {MBIA043_BLUE_CH, 12},
        {MBIA043_BLUE_CH, 13},
        {MBIA043_UNUSED_CH, 14},
        {MBIA043_UNUSED_CH, 15},
    },
};

static uint32_t     LEDA_GPIO_ROW_PINS[MATRIX_ROWS] = LED_ROW_PINS;
static unsigned int LED_ROW_NUM                     = 0;

/* Disable row pins by setting to Hi-Z */
static void mbia043_reset_row_pins(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        writePinHigh(LEDA_GPIO_ROW_PINS[i]);
    }
    return;
}

/* Flush a row's RGB values to MBIA043s starting with output channel 16 to
 * buffers. Requires GLOBAL_LATCH instruction afterwards to update comparators
 * from buffers.
 */
static inline void mbia043_write_color_row(int row) {
    writePinLow(MBIA043_LE_PIN);
    uint8_t color_ch;
    uint8_t col;
    uint8_t C_index;
    uint8_t C_mask;
    uint8_t B_index;
    uint8_t B_mask;
    uint8_t A_index;
    uint8_t A_mask;
    for (int i = MBIA043_NUM_CHANNELS - 1; i >= 0; i--) {
        // MBIA C
        color_ch = mbia043_channels[2][i].color_ch;
        col      = mbia043_channels[2][i].color_index;
        C_index  = led_matrix_co[row][col];
        C_mask   = mbia043_leds[C_index].mask;
        switch (color_ch) {
            case MBIA043_RED_CH:
                mbia043_shift_data((mbia043_leds[C_index].r & C_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
                break;
            case MBIA043_GREEN_CH:
                mbia043_shift_data((mbia043_leds[C_index].g & C_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
                break;
            case MBIA043_BLUE_CH:
                mbia043_shift_data((mbia043_leds[C_index].b & C_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
                break;
            default:
                mbia043_shift_data(0, MBIA043_SHIFT_REG_WIDTH);
        }
        // MBIA B
        color_ch = mbia043_channels[1][i].color_ch;
        col      = mbia043_channels[1][i].color_index;
        B_index  = led_matrix_co[row][col];
        B_mask   = mbia043_leds[B_index].mask;
        switch (color_ch) {
            case MBIA043_RED_CH:
                mbia043_shift_data((mbia043_leds[B_index].r & B_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
                break;
            case MBIA043_GREEN_CH:
                mbia043_shift_data((mbia043_leds[B_index].g & B_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
                break;
            case MBIA043_BLUE_CH:
                mbia043_shift_data((mbia043_leds[B_index].b & B_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
                break;
            default:
                mbia043_shift_data(0, MBIA043_SHIFT_REG_WIDTH);
        }
        // MBIA A
        color_ch = mbia043_channels[0][i].color_ch;
        col      = mbia043_channels[0][i].color_index;
        A_index  = led_matrix_co[row][col];
        A_mask   = mbia043_leds[A_index].mask;
        switch (color_ch) {
            case MBIA043_RED_CH:
                mbia043_shift_data_instr((mbia043_leds[A_index].r & A_mask) << 8, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
                break;
            case MBIA043_GREEN_CH:
                mbia043_shift_data_instr((mbia043_leds[A_index].g & A_mask) << 8, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
                break;
            case MBIA043_BLUE_CH:
                mbia043_shift_data_instr((mbia043_leds[A_index].b & A_mask) << 8, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
                break;
            default:
                mbia043_shift_data_instr(0, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
        }
    }
    writePinLow(MBIA043_SDI_PIN);
    writePinLow(MBIA043_DCLK_PIN);
    return;
}

void mbia043_set_mask(int index, uint8_t value) {
    mbia043_leds[index].mask = value;
    return;
}

static void mbia043_set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    mbia043_leds[index].r = r;
    mbia043_leds[index].g = g;
    mbia043_leds[index].b = b;
    return;
}

static void mbia043_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        mbia043_leds[i].r = r;
        mbia043_leds[i].g = g;
        mbia043_leds[i].b = b;
    }
    return;
}

static void mbia043_flush(void) {
    return;
}

/* BFTM1 timer routine to update/flush RGB values one row at a time */
static void timer_callback(GPTDriver *gptp) {
    mbia043_reset_row_pins();
    writePinLow(LEDA_GPIO_ROW_PINS[LED_ROW_NUM]);
    mbia043_send_instruction(MBIA043_GLOBAL_LATCH);
    LED_ROW_NUM = (LED_ROW_NUM + 1) & 0x7;
    mbia043_write_color_row(LED_ROW_NUM);
    return;
}

/* BFTM1 timer configuration:
 * - Input frequency = 72 MHz
 * - Compare value = 60,000 ticks
 */
static const GPTConfig BFTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .callback  = timer_callback,
};

/* GPTM1 PWM configuration:
 * - Input frequency = 72 MHz
 * - Output frequency = 3.6 MHz
 */
static const PWMConfig GPTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .period    = (HT32_CK_AHB_FREQUENCY / 3600000) - 1,
    .callback  = NULL,
    .channels =
        {
            [0] =
                {
                    .mode     = PWM_OUTPUT_ACTIVE_LOW,
                    .callback = NULL,
                },
        },
};

void mbia043_init(void) {
    /* Configure MBIA pins */
    setPinOutput(MBIA043_DCLK_PIN);
    setPinOutput(MBIA043_GCLK_PIN);
    setPinOutput(MBIA043_LE_PIN);
    setPinOutput(MBIA043_SDI_PIN);
    palSetLineMode(MBIA043_DCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_ALTERNATE(AFIO_GPIO));
    palSetLineMode(MBIA043_LE_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_ALTERNATE(AFIO_GPIO));
    palSetLineMode(MBIA043_SDI_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_ALTERNATE(AFIO_GPIO));
    writePinHigh(MBIA043_DCLK_PIN);
    writePinHigh(MBIA043_LE_PIN);
    writePinHigh(MBIA043_SDI_PIN);
    setPinInput(MBIA043_SDO_PIN);
    palSetLineMode(MBIA043_SDO_PIN, PAL_MODE_INPUT | PAL_MODE_ALTERNATE(AFIO_GPIO));

    for (int i = 0; i < MATRIX_ROWS; i++) {
        setPinOutput(LEDA_GPIO_ROW_PINS[i]);
        palSetLineMode(LEDA_GPIO_ROW_PINS[i], PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_ALTERNATE(AFIO_GPIO));
        writePinHigh(LEDA_GPIO_ROW_PINS[i]);
    }

#    ifdef MBIA043_HAS_POWER_PIN
    /* Power on MBIA */
    setPinOutput(MBIA043_PWRCTRL_PIN);
    writePinLow(MBIA043_PWRCTRL_PIN);
#    endif

    /* Start/configure PWM (at GCLK pin) */
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    palSetLineMode(MBIA043_GCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_ALTERNATE(AFIO_TM));

    int len = 0;
    /* Wait until shift register becomes ready */
    while (len != MBIA043_NUM_CASCADE * MBIA043_SHIFT_REG_WIDTH) {
        len = mbia043_get_shift_register_length();
    }

    /* Set configuration */
    uint16_t mbia043_config[MBIA043_NUM_CASCADE] = MBIA043_CONFIGURATION;
    mbia043_write_configuration(mbia043_config);

    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        mbia043_leds[i].mask = 0xff;
    }

    /* Start PWM and BFTM1 */
    pwmEnableChannel(&PWMD_GPTM1, 0, PWM_FRACTION_TO_WIDTH(&PWMD_GPTM1, 2, 1));
    gptStart(&GPTD_BFTM1, &BFTM1_config);
    if (GPTD_BFTM1.state == GPT_READY) {
        gptStartContinuous(&GPTD_BFTM1, 60000UL);
    }

    return;
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init          = mbia043_init,
    .flush         = mbia043_flush,
    .set_color     = mbia043_set_color,
    .set_color_all = mbia043_set_color_all,
};

/* Send 'instr' number of DCLK pulses while LE is asserted high. */
void inline mbia043_send_instruction(int instr) {
    writePinLow(MBIA043_LE_PIN);
    __NOP();
    __NOP();
    __NOP();
    writePinHigh(MBIA043_LE_PIN);
    while (instr-- > 0) {
        __NOP();
        __NOP();
        __NOP();
        writePinLow(MBIA043_DCLK_PIN);
        __NOP();
        __NOP();
        __NOP();
        writePinHigh(MBIA043_DCLK_PIN);
    }
    writePinLow(MBIA043_LE_PIN);
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses.
 *
 * Note: Transmission begins with MSB at data[15].
 */
void inline mbia043_shift_data(uint16_t data, int shift_amount) {
    while (shift_amount-- > 0) {
        __NOP();
        __NOP();
        __NOP();
        writePinLow(MBIA043_DCLK_PIN);
        // set SDI to data[15]
        writePin(MBIA043_SDI_PIN, data & 0x8000);
        __NOP();
        __NOP();
        __NOP();
        // clock in data
        writePinHigh(MBIA043_DCLK_PIN);
        data = (data & 0x7fff) << 1;
    }
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses,
 * and assert LE for the last instr number of DCLK pulses.
 *
 * Note: Assumes instr is less than shift_amount.
 * Note: Transmission begins with MSB at data[15].
 */
void mbia043_shift_data_instr(uint16_t data, int shift_amount, int instr) {
    if (instr < shift_amount) {
        writePinLow(MBIA043_LE_PIN);
        mbia043_shift_data(data, shift_amount - instr);
        data = data << (shift_amount - instr);
        writePinHigh(MBIA043_LE_PIN);
        mbia043_shift_data(data, instr);
        writePinLow(MBIA043_LE_PIN);
    }
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses,
 * and read shift_amount bits of data from (last-in-cascade) shift-register.
 *
 * Note: Transmission begins with MSB at data[15].
 */
uint16_t mbia043_shift_recv(uint16_t data, int shift_amount) {
    uint16_t recv = 0;
    while (shift_amount-- > 0) {
        __NOP();
        __NOP();
        __NOP();
        writePinLow(MBIA043_DCLK_PIN);
        recv = (recv << 1) | readPin(MBIA043_SDO_PIN);
        // set SDI to data[15]
        writePin(MBIA043_SDI_PIN, data & 0x8000);
        __NOP();
        __NOP();
        __NOP();
        // clock in data
        writePinHigh(MBIA043_DCLK_PIN);
        data = (data & 0x7fff) << 1;
    }
    return recv;
}

/* Read configuration data from cascaded MBIA043s.
 *
 * Note: order of array follows order from SDO to SDI pins.
 */
void mbia043_read_configuration(uint16_t *dst) {
    mbia043_send_instruction(MBIA043_READ_CONFIGURATION);
    for (int i = 0; i < MBIA043_NUM_CASCADE; i++) {
        dst[i] = mbia043_shift_recv(0, MBIA043_SHIFT_REG_WIDTH) & 0x3ff;
    }
    return;
}

/* Write configuration data to each MBIA043.
 *
 * Note: order of array follows order from SDO to SDI pins.
 */
void mbia043_write_configuration(uint16_t *src) {
    mbia043_send_instruction(MBIA043_ENABLE_WRITE_CONFIGURATION);
    int i = 0;
    for (; i < MBIA043_NUM_CASCADE - 1; i++) {
        mbia043_shift_data(src[i] << 6, MBIA043_SHIFT_REG_WIDTH);
    }
    if (i < MBIA043_NUM_CASCADE) {
        mbia043_shift_data_instr(src[i] << 6, MBIA043_SHIFT_REG_WIDTH, MBIA043_WRITE_CONFIGURATION);
    }
    return;
}

/* Find length of shift-register by clearing shift-register with 0s, writing
 * with 1s, and checking how many DCLK pulses until a 1 is detected.
 */
int mbia043_get_shift_register_length(void) {
    int len = 0;
    writePinLow(MBIA043_LE_PIN);
    // clear shift register
    mbia043_shift_data(0, 3 * MBIA043_NUM_CASCADE * MBIA043_SHIFT_REG_WIDTH);
    // write 1s until 1 appears on SDO
    int out = readPin(MBIA043_SDO_PIN);
    while (!out) {
        len++;
        mbia043_shift_data(1U << 15, 1);
        out = readPin(MBIA043_SDO_PIN);
    }
    return len;
}
#endif
