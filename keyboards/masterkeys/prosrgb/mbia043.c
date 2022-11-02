/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#include "hal.h"
#include "gpio.h"
#include "mbia043.h"
#include "quantum.h"

#ifdef RGB_MATRIX_ENABLE
typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} mbia043_led_t;

static mbia043_led_t mbia043_leds[MATRIX_ROWS * MBIA043_NUM_CHANNELS];

static uint32_t LEDA_GPIO_ROW_PINS[MATRIX_ROWS] = LED_ROW_PINS;
static unsigned int LED_ROW_NUM = 0;

/* Disable row pins by setting to tristate */
static void mbia043_reset_row_pins(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        setPinInput(LEDA_GPIO_ROW_PINS[i]);
    }
    return;
}

/* Flush a row's RGB values to MBIA043s starting with output channel 16
 * to buffers. Requires GLOBAL_LATCH instruction afterwards to update
 * comparators from buffers.
 *
 * Assumes number of channels > number of columns.
 */
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

static void mbia043_flush(void) {
    return;
}

/* BFTM0 timer routine to update/flush RGB values one row at a time */
static void timer_callback(GPTDriver *gptp) {
    mbia043_reset_row_pins();
    mbia043_send_instruction(MBIA043_GLOBAL_LATCH);
    setPinOutput(LEDA_GPIO_ROW_PINS[LED_ROW_NUM]);
    writePinLow(LEDA_GPIO_ROW_PINS[LED_ROW_NUM]);
    LED_ROW_NUM = (LED_ROW_NUM + 1) & 0x7;
    mbia043_write_color_row(LED_ROW_NUM);
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
    writePinHigh(MBIA043_DCLK_PIN);
    writePinHigh(MBIA043_LE_PIN);
    writePinHigh(MBIA043_SDI_PIN);
    setPinInput(MBIA043_SDO_PIN);

#    ifdef MBIA043_HAS_POWER_PIN
    /* Power on MBIA */
    setPinOutput(MBIA043_PWRCTRL_PIN);
    writePinHigh(MBIA043_PWRCTRL_PIN);
#    endif

    /* Start/configure PWM (at GCLK pin) */
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    writePinHigh(MBIA043_GCLK_PIN);
    palSetLineMode(MBIA043_GCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_HT32_AF(AFIO_TM));

    int len = 0;
    /* Wait until shift register becomes ready */
    while (len != MBIA043_NUM_CASCADE * MBIA043_SHIFT_REG_WIDTH) {
        len = mbia043_get_shift_register_length();
    }

    /* Set configuration */
    uint16_t mbia043_config[MBIA043_NUM_CASCADE] = {0xc, 0xc, 0xc};
    mbia043_write_configuration(mbia043_config);

    /* Start PWM and BFTM0 */
    pwmEnableChannel(&PWMD_GPTM1, 0, PWM_FRACTION_TO_WIDTH(&PWMD_GPTM1, 2, 1));
    gptStart(&GPTD_BFTM0, &BFTM0_config);
    if (GPTD_BFTM0.state == GPT_READY) {
        gptStartContinuous(&GPTD_BFTM0, 60000UL);
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
