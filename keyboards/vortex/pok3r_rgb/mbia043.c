/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#include "hal.h"
#include "gpio.h"
#include "mbia043.h"
#include "quantum.h"

#ifdef RGB_MATRIX_ENABLE
#define NLD NO_LED

// LED Matrix to LED Index
uint8_t led_matrix_co[MATRIX_ROWS][MATRIX_COLS] = {
    /*          Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8*/
    /* TODO: this assumes LED Matrix == Key Matrix; adjust if necessary */
    /* LED index is ordered by physical position (left-to-right, top-to-bottom) */
    /*Row1*/ {  0,   1,   2,   3,   4,   5,   6,   7    },
    /*Row2*/ {  14,  15,  16,  17,  18,  19,  20,  21   },
    /*Row3*/ {  28,  29,  30,  31,  32,  33,  34,  35   },
    /*Row4*/ {  41,  43,  44,  45,  46,  47,  48,  49   },
    /*Row5*/ {  55,  56,  57,  8,   9,   10,  11,  13   },
    /*Row6*/ {  42,  NLD, 12,  22,  23,  24,  25,  27   },
    /*Row7*/ {  NLD, NLD, 26,  36,  37,  38,  39,  40   },
    /*Row8*/ {  NLD, NLD, 58,  50,  51,  52,  53,  54   },
    /*Row9*/ {  NLD, NLD, NLD, NLD, 59,  60,  NLD, 61   },
    /*Row10*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
};

// clang-format off
led_config_t g_led_config = {
    { // KEY Matrix to LED Index
    /*          Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8*/
    /* LED index is ordered by physical position (left-to-right, top-to-bottom) */
    /*Row1*/ {  0,   1,   2,   3,   4,   5,   6,   7    },
    /*Row2*/ {  14,  15,  16,  17,  18,  19,  20,  21   },
    /*Row3*/ {  28,  29,  30,  31,  32,  33,  34,  35   },
    /*Row4*/ {  41,  43,  44,  45,  46,  47,  48,  49   },
    /*Row5*/ {  55,  56,  57,  8,   9,   10,  11,  13   },
    /*Row6*/ {  42,  NLD, 12,  22,  23,  24,  25,  27   },
    /*Row7*/ {  NLD, NLD, 26,  36,  37,  38,  39,  40   },
    /*Row8*/ {  NLD, NLD, 58,  50,  51,  52,  53,  54   },
    /*Row9*/ {  NLD, NLD, NLD, NLD, 59,  60,  NLD, 61   },
    /*Row10*/{  NLD, NLD, NLD, NLD, NLD, NLD, NLD, NLD  },
    }, { // LED Index to Physical Position // TODO fix positions for 60%
        {  0,   0}, { 21,   0}, { 31,   0}, { 42,   0}, { 52,   0}, { 68,   0}, { 78,   0}, { 89,   0}, { 99,   0}, {115,   0}, {125,   0}, {135,   0}, {146,   0}, {159,   0},
        {  0,  17}, { 10,  17}, { 21,  17}, { 31,  17}, { 42,  17}, { 52,  17}, { 63,  17}, { 73,  17}, { 83,  17}, { 94,  17}, {104,  17}, {115,  17}, {125,  17}, {141,  17},
        {  3,  29}, { 16,  29}, { 26,  29}, { 36,  29}, { 47,  29}, { 57,  29}, { 68,  29}, { 78,  29}, { 89,  29}, { 99,  29}, {109,  29}, {120,  29}, {130,  29},
        {  4,  41}, { 18,  41}, { 29,  41}, { 39,  41}, { 49,  41}, { 60,  41}, { 70,  41}, { 81,  41}, { 91,  41}, {102,  41}, {112,  41}, {122,  41}, {139,  41},
        {  1,  64}, { 14,  64}, { 27,  64}, { 66,  64}, {105,  64}, {119,  64}, {132,  64}, {145,  64},
    }, { // LED Index to Flag
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    4,
        4, 4, 4,          4,          4, 4, 4, 4,
    }
};
// clang-format on

typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t mask;
} mbia043_led_t;

static mbia043_led_t mbia043_leds[DRIVER_LED_TOTAL];

static uint32_t LEDA_GPIO_COL_PINS[MATRIX_COLS] = LED_COL_PINS;
static unsigned int LED_COL_NUM = 0;

/* Disable column pins by setting to tristate */
static void mbia043_reset_col_pins(void) {
    for (int i = 0; i < MATRIX_COLS; i++) {
        setPinInput(LEDA_GPIO_COL_PINS[i]);
    }
    return;
}

/* Flush a column's RGB values to MBIA043s starting with output channel 16 to
 * buffers. Requires GLOBAL_LATCH instruction afterwards to update comparators
 * from buffers.
 *
 * TODO: adjust channel assignment if necessary since this is all guesswork and untested.
 *
 * Assumed configuration:
 *          _______
 *    SDI->|       |->SDO1_2
 * R_ROW1<-|       |->unused (?)
 * G_ROW1<-|       |->B_ROW5
 * B_ROW1<-|  1st  |->G_ROW5
 * R_ROW2<-|MBIA043|->R_ROW5
 * G_ROW2<-|   A   |->B_ROW4
 * B_ROW2<-|       |->G_ROW4
 * R_ROW3<-|       |->R_ROW4
 * G_ROW3<-|_______|->B_ROW3
 *          _______
 * SDO1_2->|       |->SDO
 * R_ROW6<-|       |->unused (?)
 * G_ROW6<-|       |->B_ROW10
 * B_ROW6<-|  2nd  |->G_ROW10
 * R_ROW7<-|MBIA043|->R_ROW10
 * G_ROW7<-|   B   |->B_ROW9
 * B_ROW7<-|       |->G_ROW9
 * R_ROW8<-|       |->R_ROW9
 * G_ROW8<-|_______|->B_ROW8
 *
 */
static inline void mbia043_write_color_col(int col) {
    palClearLine(MBIA043_LE_PIN);
    // set 0 to unused channels
    mbia043_shift_data(0, MBIA043_SHIFT_REG_WIDTH);
    mbia043_shift_data_instr(0, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
    // start from B_ROW10 & B_ROW5...
    for (int i = MATRIX_ROWS - 1; i >= MATRIX_ROWS / 2; i--) {
        uint8_t B_index = led_matrix_co[i][col];
        uint8_t A_index = led_matrix_co[i-5][col];
        uint8_t B_mask = mbia043_leds[B_index].mask;
        uint8_t A_mask = mbia043_leds[A_index].mask;

        // Blue
        // Row i of 2nd MBIA IC
        mbia043_shift_data((mbia043_leds[B_index].b & B_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
        // Row i-5 of 2nd MBIA IC
        mbia043_shift_data_instr((mbia043_leds[A_index].b & A_mask) << 8, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);

        // Green
        // Row i of 2nd MBIA IC
        mbia043_shift_data((mbia043_leds[B_index].g & B_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
        // Row i-5 of 2nd MBIA IC
        mbia043_shift_data_instr((mbia043_leds[A_index].g & A_mask) << 8, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);

        // Red
        // Row i of 2nd MBIA IC
        mbia043_shift_data((mbia043_leds[B_index].r & B_mask) << 8, MBIA043_SHIFT_REG_WIDTH);
        // Row i-5 of 2nd MBIA IC
        mbia043_shift_data_instr((mbia043_leds[A_index].r & A_mask) << 8, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
    }
    palClearLine(MBIA043_SDI_PIN);
    palClearLine(MBIA043_DCLK_PIN);
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
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        mbia043_leds[i].r = r;
        mbia043_leds[i].g = g;
        mbia043_leds[i].b = b;
    }
    return;
}

static void mbia043_flush(void) {
    return;
}

/* BFTM0 timer routine to update/flush RGB values one row at a time */
static void timer_callback(GPTDriver *gptp) {
    mbia043_reset_col_pins();
    mbia043_send_instruction(MBIA043_GLOBAL_LATCH);
    setPinOutput(LEDA_GPIO_COL_PINS[LED_COL_NUM]);
    palClearLine(LEDA_GPIO_COL_PINS[LED_COL_NUM]);
    LED_COL_NUM = (LED_COL_NUM + 1) & 0x7;
    mbia043_write_color_col(LED_COL_NUM);
    return;
}

/* BFTM0 timer configuration:
 * - Input frequency = 72 MHz
 * - Compare value = 60,000 ticks
 */
static const GPTConfig BFTM0_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .callback  = timer_callback,
};

/* GPTM1 PWM configuration:
 * - Input frequency = 72 MHz
 * - Output frequency = 14.4 MHz
 */
static const PWMConfig GPTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .period    = (HT32_CK_AHB_FREQUENCY / 14400000) - 1,
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
    palSetLine(MBIA043_DCLK_PIN);
    palSetLine(MBIA043_LE_PIN);
    palSetLine(MBIA043_SDI_PIN);
    setPinInput(MBIA043_SDO_PIN);

#    ifdef MBIA043_HAS_POWER_PIN
    /* Power on MBIA */
    setPinOutput(MBIA043_PWRCTRL_PIN);
    // Set this to palClearLine if pin is connected to pmos instead of nmos
    palSetLine(MBIA043_PWRCTRL_PIN);
#    endif

    /* Start/configure PWM (at GCLK pin) */
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    palSetLine(MBIA043_GCLK_PIN);
    palSetLineMode(MBIA043_GCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_ALTERNATE(AFIO_TM));

    int len = 0;
    /* Wait until shift register becomes ready */
    while (len != MBIA043_NUM_CASCADE * MBIA043_SHIFT_REG_WIDTH) {
        len = mbia043_get_shift_register_length();
    }

    /* Set configuration */
    uint16_t mbia043_config[MBIA043_NUM_CASCADE] = {0xc, 0xc};
    mbia043_write_configuration(mbia043_config);

    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        mbia043_leds[i].mask = 0xff;
    }

    /* Start PWM and BFTM0 */
    pwmEnableChannel(&PWMD_GPTM1, 0, PWM_FRACTION_TO_WIDTH(&PWMD_GPTM1, 2, 1));
    gptStart(&GPTD_BFTM0, &BFTM0_config);
    if (GPTD_BFTM0.state == GPT_READY) {
        gptStartContinuous(&GPTD_BFTM0, 60000UL);
    }

    // TODO: remove test
    // This test is used to check the led matrix by lighting each LED in order of
    // physical position (left-to-right, top-to-bottom).
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        mbia043_set_color(i, 20+i, 20+i, 20+i);
        printf("%d\n", i);
        for (int j = 0; j < 1; j++) {
            //reload_watchdog();
            wait_ms(100);
        }
    }

    return;
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init          = mbia043_init,
    .flush         = mbia043_flush,
    .set_color     = mbia043_set_color,
    .set_color_all = mbia043_set_color_all,
};
#endif

/* Send 'instr' number of DCLK pulses while LE is asserted high. */
void inline mbia043_send_instruction(int instr) {
    palClearLine(MBIA043_LE_PIN);
    __NOP();
    __NOP();
    __NOP();
    palSetLine(MBIA043_LE_PIN);
    while (instr-- > 0) {
        __NOP();
        __NOP();
        __NOP();
        palClearLine(MBIA043_DCLK_PIN);
        __NOP();
        __NOP();
        __NOP();
        palSetLine(MBIA043_DCLK_PIN);
    }
    palClearLine(MBIA043_LE_PIN);
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
        palClearLine(MBIA043_DCLK_PIN);
        // set SDI to data[15]
        if (data & 0x8000)
            palSetLine(MBIA043_SDI_PIN);
        else
            palClearLine(MBIA043_SDI_PIN);
        __NOP();
        __NOP();
        __NOP();
        // clock in data
        palSetLine(MBIA043_DCLK_PIN);
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
        palClearLine(MBIA043_LE_PIN);
        mbia043_shift_data(data, shift_amount - instr);
        data = data << (shift_amount - instr);
        palSetLine(MBIA043_LE_PIN);
        mbia043_shift_data(data, instr);
        palClearLine(MBIA043_LE_PIN);
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
        palClearLine(MBIA043_DCLK_PIN);
        recv = (recv << 1) | readPin(MBIA043_SDO_PIN);
        // set SDI to data[15]
        if (data & 0x8000)
            palSetLine(MBIA043_SDI_PIN);
        else
            palClearLine(MBIA043_SDI_PIN);
        __NOP();
        __NOP();
        __NOP();
        // clock in data
        palSetLine(MBIA043_DCLK_PIN);
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
    palClearLine(MBIA043_LE_PIN);
    // clear shift register
    mbia043_shift_data(0, MBIA043_NUM_CASCADE * MBIA043_SHIFT_REG_WIDTH);
    // write 1s until 1 appears on SDO
    int out = readPin(MBIA043_SDO_PIN);
    while (!out) {
        len++;
        mbia043_shift_data(1U << 15, 1);
        out = readPin(MBIA043_SDO_PIN);
    }
    return len;
}
