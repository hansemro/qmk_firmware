/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#include "hal.h"
#include "gpio.h"
#include "mbia043.h"
#include "quantum.h"

#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {
    { // KEY Matrix to LED Index
    //      Col1 Col2 Col3 Col4 Col5 Col6 Col7      Col8    Col9    Col10   Col11   Col12   Col13   Col14   Col15
/*Row1*/{   16,  18,  20,  22,  24,  26,  28,       30,     32,     NO_LED, NO_LED, NO_LED, 0,      4,      7},
/*Row2*/{   17,  19,  21,  23,  25,  27,  29,       31,     NO_LED, NO_LED, NO_LED, NO_LED, 1,      5,      8},
/*Row3*/{   33,  35,  37,  39,  41,  43,  45,       47,     49,     NO_LED, NO_LED, 77,     2,      6,      9},
/*Row4*/{   34,  36,  38,  40,  42,  44,  46,       48,     10,     NO_LED, NO_LED, 76,     3,      15,     NO_LED},
/*Row5*/{   50,  52,  54,  56,  58,  60,  62,       82,     11,     NO_LED, NO_LED, 78,     NO_LED, NO_LED, NO_LED},
/*Row6*/{   51,  53,  55,  57,  59,  61,  NO_LED,   75,     12,     NO_LED, NO_LED, 79,     NO_LED, NO_LED, NO_LED},
/*Row7*/{   63,  65,  67,  69,  71,  73,  83,       85,     86,     NO_LED, NO_LED, 80,     13,     NO_LED, NO_LED},
/*Row8*/{   64,  66,  68,  70,  72,  74,  NO_LED,   84,     NO_LED, NO_LED, NO_LED, 81,     14,     NO_LED, NO_LED},
    }, { // LED Index to Physical Position
        {  0,   0}, { 26,   0}, { 39,   0}, { 52,   0}, { 65,   0}, { 84,   0}, { 97,   0}, {110,   0}, {123,   0}, {143,   0}, {156,   0}, {169,   0}, {182,   0}, {198,   0}, {211,   0}, {224,   0},
        {  0,  17}, { 13,  17}, { 26,  17}, { 39,  17}, { 52,  17}, { 65,  17}, { 78,  17}, { 91,  17}, {104,  17}, {117,  17}, {130,  17}, {143,  17}, {156,  17}, {175,  17}, {198,  17}, {211,  17}, {224,  17},
        {  3,  29}, { 19,  29}, { 32,  29}, { 45,  29}, { 58,  29}, { 71,  29}, { 84,  29}, { 97,  29}, {110,  29}, {123,  29}, {136,  29}, {149,  29}, {162,  29}, {172,  29}, {198,  29}, {211,  29}, {224,  29},
        {  5,  41}, { 23,  41}, { 36,  41}, { 49,  41}, { 62,  41}, { 75,  41}, { 88,  41}, {101,  41}, {114,  41}, {127,  41}, {140,  41}, {153,  41}, {174,  41},
        {  8,  52}, { 29,  52}, { 42,  52}, { 55,  52}, { 68,  52}, { 81,  52}, { 94,  52}, {107,  52}, {120,  52}, {133,  52}, {146,  52}, {170,  52}, {211,  52},
        {  2,  64}, { 18,  64}, { 34,  64}, { 83,  64}, {131,  64}, {148,  64}, {164,  64}, {180,  64}, {198,  64}, {211,  64}, {224,  64},
    }, { // LED Index to Flag
        4,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    4,
        1,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1,     4,
        1, 1, 1,          4,          1, 1, 1, 1,  4, 4, 4,
    }
};

typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} mbia043_led_t;

mbia043_led_t mbia043_leds[MATRIX_ROWS * MBIA043_NUM_CHANNELS];

static uint32_t LEDA_GPIO_ROW_PINS[MATRIX_ROWS] = {
    Q8_Ctrl,
    Q1_Ctrl,
    Q2_Ctrl,
    Q3_Ctrl,
    Q4_Ctrl,
    Q5_Ctrl,
    Q6_Ctrl,
    Q7_Ctrl,
};
static unsigned int LED_ROW_NUM = 0;

/* Disable row pins by setting to tristate */
static void mbia043_reset_row_pins(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        setPinInput(LEDA_GPIO_ROW_PINS[i]);
    }
    return;
}

/* Flush a row's RGB values to MBIA043s starting with output channel 16
 * to buffers. Requires OVERALL_LATCH instruction afterwards to update
 * comparators from buffers.
 * */
static inline void mbia043_write_color_row(int row) {
    writePinLow(MBIA043_LE_PIN);
    for (int i = 0; i < MBIA043_NUM_CHANNELS - MATRIX_COLS; i++) {
        mbia043_shift_data(0, MBIA043_SHIFT_REG_WIDTH);
        mbia043_shift_data(0, MBIA043_SHIFT_REG_WIDTH);
        mbia043_shift_data_instr(0, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
    }
    for (int i = MATRIX_COLS - 1; i >= 0; i--) {
        mbia043_shift_data(mbia043_leds[g_led_config.matrix_co[row][i]].b << 8, MBIA043_SHIFT_REG_WIDTH);
        mbia043_shift_data(mbia043_leds[g_led_config.matrix_co[row][i]].g << 8, MBIA043_SHIFT_REG_WIDTH);
        mbia043_shift_data_instr(mbia043_leds[g_led_config.matrix_co[row][i]].r << 8, MBIA043_SHIFT_REG_WIDTH, MBIA043_DATA_LATCH);
    }
    writePinLow(MBIA043_SDI_PIN);
    writePinLow(MBIA043_DCLK_PIN);
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

static void mbia043_flush(void) { return; }

/* BFTM1 timer routine to update/flush RGB values one row at a time */
static void timer_callback(GPTDriver *gptp) {
    mbia043_reset_row_pins();
    mbia043_send_instruction(MBIA043_GLOBAL_LATCH);
    setPinOutput(LEDA_GPIO_ROW_PINS[LED_ROW_NUM]);
    writePinLow(LEDA_GPIO_ROW_PINS[LED_ROW_NUM]);
    mbia043_write_color_row(LED_ROW_NUM);
    LED_ROW_NUM = (LED_ROW_NUM + 1) & 0x7;
    return;
}

/* BFTM1 timer configuration:
 * - Input frequency = 72 MHz
 * - Compare value = 60,000 ticks
 */
static const GPTConfig BFTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .callback = timer_callback,
};

/* GPTM1 PWM configuration:
 * - Input frequency = 72 MHz
 * - Output frequency = 3.6 MHz
 */
static const PWMConfig GPTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .period = (HT32_CK_AHB_FREQUENCY / 3600000) - 1,
    .callback = NULL,
    .channels = {
        [0] = {
            .mode = PWM_OUTPUT_ACTIVE_LOW,
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
    writePinHigh(MBIA043_DCLK_PIN);
    writePinHigh(MBIA043_LE_PIN);
    writePinHigh(MBIA043_SDI_PIN);
    setPinInput(MBIA043_SDO_PIN);

#ifdef MBIA043_HAS_POWER_PIN
    /* Power on MBIA */
    setPinOutput(MBIA043_PWRCTRL_PIN);
    writePinHigh(MBIA043_PWRCTRL_PIN);
#endif

    /* Start/configure PWM (at GCLK pin) */
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    writePinHigh(MBIA043_GCLK_PIN);
    palSetLineMode(MBIA043_GCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL |
            PAL_MODE_HT32_AF(AFIO_TM));

    int len = 0;
    /* Wait until shift register becomes ready */
    while (len != MBIA043_NUM_CASCADE * MBIA043_SHIFT_REG_WIDTH) {
        len = mbia043_get_shift_register_length();
    }

    /* Set configuration */
    uint16_t mbia043_config[MBIA043_NUM_CASCADE] = { 0xc, 0xc, 0xc };
    mbia043_write_configuration(mbia043_config);

    /* Start PWM and BFTM1 */
    pwmEnableChannel(&PWMD_GPTM1, 0, PWM_FRACTION_TO_WIDTH(&PWMD_GPTM1, 2, 1));
    gptStart(&GPTD_BFTM1, &BFTM1_config);
    if (GPTD_BFTM1.state == GPT_READY) {
        gptStartContinuous(&GPTD_BFTM1, 60000UL);
    }
    return;
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init = mbia043_init,
    .flush = mbia043_flush,
    .set_color = mbia043_set_color,
    .set_color_all = mbia043_set_color_all,
};
#endif

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
        mbia043_shift_data(data, shift_amount-instr);
        data = data << (shift_amount-instr);
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
        mbia043_shift_data_instr(src[i] << 6, MBIA043_SHIFT_REG_WIDTH,
                MBIA043_WRITE_CONFIGURATION);
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
