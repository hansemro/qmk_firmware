// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "hal.h"
#include "wait.h"

#define SRAM_BOOT_MODE 0x40080304
#define RESET_AP_MAGIC 0x55aafaf0
#define RESET_IAP_MAGIC 0x55aafaf5

void bootloader_jump(void) {
    wait_us(10000);                // 10 ms
    chSysDisable();                // mask all interrupts
    usbDisconnectBus(&USB_DRIVER); // disconnect usb
    *((volatile uint32_t *)SRAM_BOOT_MODE) = RESET_IAP_MAGIC;
    wait_us(50000); // 50 ms
    NVIC_SystemReset();
}

#ifdef RGB_MATRIX_ENABLE
#    include "mbi.h"

typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} mbia043_led_t;

// mbia043_leds[0]: back buffer
// mbia043_leds[1]: front buffer
static mbia043_led_t mbia043_leds[2][MATRIX_ROWS * MBI_NUM_CHANNELS] = {0};

static uint32_t LED_GPIO_ROW_PINS[MATRIX_ROWS] = LED_ROW_PINS;
static uint8_t  LED_ROW_IDX                    = 0;

/* Disable row pins by setting to Hi-Z */
static void mbia043_reset_row_pins(void) {
    for (int i = 0; i < MATRIX_ROWS; i++) {
        writePinHigh(LED_GPIO_ROW_PINS[i]);
    }
    return;
}

/* Flush a row's RGB values to MBIA043s starting with output channel 16
 * to buffers. Requires GLOBAL_LATCH instruction afterwards to update
 * comparators from buffers.
 *
 * Assumes number of channels > number of columns.
 */
static void mbia043_write_color_row(int row) {
    writePinLow(MBI_LE_PIN);
    // Write 0s to unused buffers
    for (int i = 0; i < MBI_NUM_CHANNELS - MATRIX_COLS; i++) {
        mbi_shift_data(0, MBI_SHIFT_REG_WIDTH);
        mbi_shift_data(0, MBI_SHIFT_REG_WIDTH);
        mbi_shift_data_instr(0, MBI_SHIFT_REG_WIDTH, MBI_DATA_LATCH);
    }
    for (int i = MATRIX_COLS - 1; i >= 0; i--) {
        uint8_t index = g_led_config.matrix_co[row][i];
        mbi_shift_data((mbia043_leds[1][index].b) << 8, MBI_SHIFT_REG_WIDTH);
        mbi_shift_data((mbia043_leds[1][index].g) << 8, MBI_SHIFT_REG_WIDTH);
        mbi_shift_data_instr((mbia043_leds[1][index].r) << 8, MBI_SHIFT_REG_WIDTH, MBI_DATA_LATCH);
    }
    writePinLow(MBI_SDI_PIN);
    writePinLow(MBI_DCLK_PIN);
    return;
}

static void mbia043_set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    mbia043_leds[0][index].r = r;
    mbia043_leds[0][index].g = g;
    mbia043_leds[0][index].b = b;
    return;
}

static void mbia043_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        mbia043_leds[0][i].r = r;
        mbia043_leds[0][i].g = g;
        mbia043_leds[0][i].b = b;
    }
    return;
}

static void mbia043_flush(void) {
    memcpy(&mbia043_leds[1], &mbia043_leds[0], MATRIX_ROWS * MBI_NUM_CHANNELS * sizeof(mbia043_led_t));
    return;
}

/* BFTM0 timer routine to update/flush RGB values one row at a time */
static void mbia043_isr(GPTDriver *gptp) {
    mbia043_reset_row_pins();
    mbi_send_instruction(MBI_GLOBAL_LATCH);
    writePinLow(LEDA_GPIO_ROW_PINS[LED_ROW_IDX]);
    LED_ROW_IDX = (LED_ROW_IDX + 1) & 0x7;
    mbia043_write_color_row(LED_ROW_IDX);
    return;
}

/* BFTM0 timer configuration:
 * - Input frequency = 72 MHz
 * - Compare value = 60,000 ticks
 */
static const GPTConfig BFTM0_config = {
    .frequency = HT32_CK_AHB_FREQUENCY,
    .callback  = mbia043_isr,
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

static void mbia043_init(void) {
    /* Configure MBIA pins */
    setPinOutput(MBI_DCLK_PIN);
    setPinOutput(MBI_GCLK_PIN);
    setPinOutput(MBI_LE_PIN);
    setPinOutput(MBI_SDI_PIN);
    writePinHigh(MBI_DCLK_PIN);
    writePinHigh(MBI_LE_PIN);
    writePinHigh(MBI_SDI_PIN);
    setPinInput(MBI_SDO_PIN);

    for (int i = 0; i < MATRIX_ROWS; i++) {
        setPinOutput(LED_GPIO_ROW_PINS[i]);
        palSetLineMode(LED_GPIO_ROW_PINS[i], PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO));
        writePinHigh(LED_GPIO_ROW_PINS[i]);
    }

    /* Power on MBIA */
    setPinOutput(MBI_PWRCTRL_PIN);
    palSetLineMode(MBI_PWRCTRL_PIN, PAL_MODE_OUTPUT_OPENDRAIN | PAL_MODE_HT32_AF(AFIO_GPIO));
    writePinLow(MBI_PWRCTRL_PIN);

    /* Start/configure PWM (at GCLK pin) */
    pwmStart(&PWMD_GPTM1, &GPTM1_config);
    writePinHigh(MBI_GCLK_PIN);
    palSetLineMode(MBI_GCLK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_MODE_HT32_AF(AFIO_TM));

    int len = 0;
    /* Wait until shift register becomes ready */
    while (len != MBI_NUM_CASCADE * MBI_SHIFT_REG_WIDTH) {
        len = mbi_get_shift_register_length();
    }

    /* Set configuration */
    mbi_write_configuration(MBIA043_CONFIGURATION);

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

// Toggles RGB off for Caps and Scroll Lock keys if disabled
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (!host_keyboard_led_state().caps_lock) mbia043_set_color(50, RGB_OFF);
    if (!host_keyboard_led_state().scroll_lock) mbia043_set_color(14, RGB_OFF);
    return false;
}
#endif
