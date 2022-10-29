/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#include "hal.h"
#include "gpio.h"
#include "mbi5042.h"
#include "quantum.h"
#include "print.h"

#ifdef RGB_MATRIX_ENABLE
#define NLD NO_LED
// clang-format off
led_config_t g_led_config = {
    { // KEY Matrix to LED Index
    /*          Col1 Col2 Col3 Col4 Col5 Col6 Col7 Col8 Col9 Col10 Col11 Col12 Col13 Col14 Col15 Col16*/
    /*Row1*/{   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,    10,   11,   12,   NLD,  13,   14},
    /*Row2*/{   20,  21,  22,  23,  24,  25,  26,  28,  29,  30,   31,   32,   NLD,  89,   34,   35},
    /*Row3*/{   41,  42,  43,  44,  45,  46,  27,  48,  49,  50,   51,   52,   53,   101,  55,   56},
    /*Row4*/{   62,  63,  64,  65,  66,  67,  47,  69,  70,  71,   72,   73,   33,   74,   90,   15},
    /*Row5*/{   78,  NLD, 79,  80,  81,  82,  68,  84,  86,  87,   88,   NLD,  54,   102,  103,  36},
    /*Row6*/{   95,  96,  97,  NLD, 98,  NLD, 83,  85,  NLD, NLD,  99,   100,  NLD,  105,  104,  57},
    /*Row7*/{   NLD, NLD, NLD, NLD, NLD, 16,  17,  18,  19,  37,   38,   39,   40,   58,   59,   60},
    /*Row8*/{   NLD, NLD, NLD, NLD, NLD, 75,  76,  77,  61,  91,   92,   93,   94,   106,  107,  NLD},
    }, { // LED Index to Physical Position
        {  0,   0}, { 21,   0}, { 31,   0}, { 42,   0}, { 52,   0}, { 68,   0}, { 78,   0}, { 89,   0}, { 99,   0}, {115,   0}, {125,   0}, {135,   0}, {146,   0}, {159,   0}, {169,   0}, {180,   0}, {193,   0}, {203,   0}, {214,   0}, {224,   0},
        {  0,  17}, { 10,  17}, { 21,  17}, { 31,  17}, { 42,  17}, { 52,  17}, { 63,  17}, { 73,  17}, { 83,  17}, { 94,  17}, {104,  17}, {115,  17}, {125,  17}, {141,  17}, {159,  17}, {169,  17}, {180,  17}, {193,  17}, {203,  17}, {214,  17}, {224,  17},
        {  3,  29}, { 16,  29}, { 26,  29}, { 36,  29}, { 47,  29}, { 57,  29}, { 68,  29}, { 78,  29}, { 89,  29}, { 99,  29}, {109,  29}, {120,  29}, {130,  29}, {138,  29}, {159,  29}, {169,  29}, {180,  29}, {193,  29}, {203,  29}, {214,  29}, {224,  35},
        {  4,  41}, { 18,  41}, { 29,  41}, { 39,  41}, { 49,  41}, { 60,  41}, { 70,  41}, { 81,  41}, { 91,  41}, {102,  41}, {112,  41}, {122,  41}, {139,  41}, {193,  41}, {203,  41}, {214,  41},
        {  7,  52}, { 23,  52}, { 34,  52}, { 44,  52}, { 55,  52}, { 65,  52}, { 76,  52}, { 86,  52}, { 96,  52}, {107,  52}, {117,  52}, {137,  52}, {169,  52}, {193,  52}, {203,  52}, {214,  52}, {224,  58},
        {  1,  64}, { 14,  64}, { 27,  64}, { 66,  64}, {105,  64}, {119,  64}, {132,  64}, {145,  64}, {159,  64}, {169,  64}, {180,  64}, {198,  64}, {214,  64}
    }, { // LED Index to Flag
        4,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,  4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,  4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  4, 4, 4,  4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    4,            4, 4, 4,
        1,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1,     4,     4, 4, 4, 4,
        1, 1, 1,          4,          1, 1, 1, 1,  4, 4, 4,  4,    4,
    }
};
// clang-format on

typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} mbi_led_t;

static mbi_led_t mbi5042_leds[MATRIX_ROWS * MBI5042_NUM_CHANNELS];

static uint32_t LED_GPIO_ROW_PINS[MATRIX_ROWS] = LED_ROW_PINS;

static unsigned int LED_ROW_NUM = 0;

/* Disable row pins by setting to tristate */
static void mbi5042_reset_row_pins(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        setPinInput(LED_GPIO_ROW_PINS[i]);
    }
    return;
}

/* Flush a row's RGB values to MBI5042s starting with output channel 16
 * to buffers. Requires GLOBAL_LATCH instruction afterwards to update
 * comparators from buffers.
 *
 * Assumes number of channels > number of columns.
 */
static inline void mbi5042_write_color_row(int row) {
    writePinLow(MBI5042_LE_PIN);
    for (int i = MATRIX_COLS - 1; i >= 0; i--) {
        mbi5042_shift_data_instr(mbi5042_leds[g_led_config.matrix_co[row][i]].r << 8, MBI5042_SHIFT_REG_WIDTH, MBI5042_DATA_LATCH);
    }
    writePinLow(MBI5042_SDI_PIN);
    writePinLow(MBI5042_DCLK_PIN);
    return;
}

static void mbi5042_set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    mbi5042_leds[index].r = r;
    mbi5042_leds[index].g = g;
    mbi5042_leds[index].b = b;
    return;
}

static void mbi5042_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        mbi5042_leds[i].r = r;
        mbi5042_leds[i].g = g;
        mbi5042_leds[i].b = b;
    }
    return;
}

static void mbi5042_flush(void) {
    return;
}

/* BFTM1 timer routine to update/flush RGB values one row at a time */
static void timer_callback(GPTDriver *gptp) {
    mbi5042_reset_row_pins();
    mbi5042_send_instruction(MBI5042_GLOBAL_LATCH);
    setPinOutput(LED_GPIO_ROW_PINS[LED_ROW_NUM]);
    writePinHigh(LED_GPIO_ROW_PINS[LED_ROW_NUM]);
    LED_ROW_NUM = (LED_ROW_NUM + 1) & 0x7;
    mbi5042_write_color_row(LED_ROW_NUM);
    return;
}

/* BFTM1 timer configuration:
 * - Input frequency = 48 MHz
 * - Compare value = 30,000 ticks
 */
static const GPTConfig BFTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .callback  = timer_callback,
};

/* GPTM1 PWM configuration:
 * - Input frequency = 48 MHz
 * - Output frequency = 1.8 MHz
 */
static const PWMConfig GPTM1_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .period    = (HT32_CK_AHB_FREQUENCY / 1800000) - 1,
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

void mbi5042_init(void) {
    /* Configure MBIA pins */
    setPinOutput(MBI5042_DCLK_PIN);
    setPinOutput(MBI5042_GCLK_PIN);
    setPinOutput(MBI5042_LE_PIN);
    setPinOutput(MBI5042_SDI_PIN);
    writePinHigh(MBI5042_DCLK_PIN);
    writePinHigh(MBI5042_LE_PIN);
    writePinHigh(MBI5042_SDI_PIN);
    setPinInput(MBI5042_SDO_PIN);

#    ifdef MBI5042_HAS_POWER_PIN
    /* Power on MBIA */
    setPinOutput(MBI5042_PWRCTRL_PIN);
    writePinLow(MBI5042_PWRCTRL_PIN);
#    endif

    /* Start/configure PWM (at GCLK pin) */
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    writePinHigh(MBI5042_GCLK_PIN);
    palSetLineMode(MBI5042_GCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_HT32_AF(AFIO_TM));

    int len = 0;
    /* Wait until shift register becomes ready */
    while (len != MBI5042_NUM_CASCADE * MBI5042_SHIFT_REG_WIDTH) {
        len = mbi5042_get_shift_register_length();
    }

    /* Set configuration */
    uint16_t mbi5042_config[MBI5042_NUM_CASCADE] = {0x12c};
    mbi5042_write_configuration(mbi5042_config);

    /* Start PWM and BFTM1 */
    pwmEnableChannel(&PWMD_GPTM1, 0, PWM_FRACTION_TO_WIDTH(&PWMD_GPTM1, 2, 1));
    gptStart(&GPTD_BFTM1, &BFTM1_config);
    if (GPTD_BFTM1.state == GPT_READY) {
        gptStartContinuous(&GPTD_BFTM1, 30000UL);
    }

#if 0
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        mbi5042_set_color(i, 20+i, 0, 0);
        printf("%d\n", i);
        for (int j = 0; j < 1; j++) {
            reload_watchdog();
            wait_ms(100);
        }
    }
#endif
    return;
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init           = mbi5042_init,
    .flush          = mbi5042_flush,
    .set_color      = mbi5042_set_color,
    .set_color_all  = mbi5042_set_color_all,
};

#endif

/* Send 'instr' number of DCLK pulses while LE is asserted high. */
void inline mbi5042_send_instruction(int instr) {
    writePinLow(MBI5042_LE_PIN);
    __NOP();
    __NOP();
    writePinHigh(MBI5042_LE_PIN);
    while (instr-- > 0) {
        __NOP();
        __NOP();
        writePinLow(MBI5042_DCLK_PIN);
        __NOP();
        __NOP();
        writePinHigh(MBI5042_DCLK_PIN);
    }
    writePinLow(MBI5042_LE_PIN);
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses.
 *
 * Note: Transmission begins with MSB at data[15].
 */
void inline mbi5042_shift_data(uint16_t data, int shift_amount) {
    while (shift_amount-- > 0) {
        __NOP();
        __NOP();
        writePinLow(MBI5042_DCLK_PIN);
        // set SDI to data[15]
        writePin(MBI5042_SDI_PIN, data & 0x8000);
        __NOP();
        __NOP();
        // clock in data
        writePinHigh(MBI5042_DCLK_PIN);
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
void mbi5042_shift_data_instr(uint16_t data, int shift_amount, int instr) {
    if (instr < shift_amount) {
        writePinLow(MBI5042_LE_PIN);
        mbi5042_shift_data(data, shift_amount - instr);
        data = data << (shift_amount - instr);
        writePinHigh(MBI5042_LE_PIN);
        mbi5042_shift_data(data, instr);
        writePinLow(MBI5042_LE_PIN);
    }
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses,
 * and read shift_amount bits of data from (last-in-cascade) shift-register.
 *
 * Note: Transmission begins with MSB at data[15].
 */
uint16_t mbi5042_shift_recv(uint16_t data, int shift_amount) {
    uint16_t recv = 0;
    while (shift_amount-- > 0) {
        __NOP();
        __NOP();
        writePinLow(MBI5042_DCLK_PIN);
        recv = (recv << 1) | readPin(MBI5042_SDO_PIN);
        // set SDI to data[15]
        writePin(MBI5042_SDI_PIN, data & 0x8000);
        __NOP();
        __NOP();
        // clock in data
        writePinHigh(MBI5042_DCLK_PIN);
        data = (data & 0x7fff) << 1;
    }
    return recv;
}

/* Read configuration data from cascaded MBI5042s.
 *
 * Note: order of array follows order from SDO to SDI pins.
 */
void mbi5042_read_configuration(uint16_t *dst) {
    mbi5042_send_instruction(MBI5042_READ_CONFIGURATION);
    for (int i = 0; i < MBI5042_NUM_CASCADE; i++) {
        dst[i] = mbi5042_shift_recv(0, MBI5042_SHIFT_REG_WIDTH) & 0x3ff;
    }
    return;
}

/* Write configuration data to each MBI5042.
 *
 * Note: order of array follows order from SDO to SDI pins.
 */
void mbi5042_write_configuration(uint16_t *src) {
    mbi5042_send_instruction(MBI5042_ENABLE_WRITE_CONFIGURATION);
    int i = 0;
    for (; i < MBI5042_NUM_CASCADE - 1; i++) {
        mbi5042_shift_data(src[i] << 6, MBI5042_SHIFT_REG_WIDTH);
    }
    if (i < MBI5042_NUM_CASCADE) {
        mbi5042_shift_data_instr(src[i] << 6, MBI5042_SHIFT_REG_WIDTH, MBI5042_WRITE_CONFIGURATION);
    }
    return;
}

/* Find length of shift-register by clearing shift-register with 0s, writing
 * with 1s, and checking how many DCLK pulses until a 1 is detected.
 */
int mbi5042_get_shift_register_length(void) {
    int len = 0;
    writePinLow(MBI5042_LE_PIN);
    // clear shift register
    mbi5042_shift_data(0, MBI5042_NUM_CASCADE * MBI5042_SHIFT_REG_WIDTH);
    // write 1s until 1 appears on SDO
    int out = readPin(MBI5042_SDO_PIN);
    while (!out) {
        len++;
        mbi5042_shift_data(1U << 15, 1);
        out = readPin(MBI5042_SDO_PIN);
    }
    return len;
}
