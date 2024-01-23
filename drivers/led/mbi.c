// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "string.h"
#include "gpio.h"
#include "mbi.h"

#ifdef SPLIT_KEYBOARD
#    error "Split keyboard not supported"
#endif

pin_t g_led_pins[MBI_NUM_LED_GPIO_PINS] = MBI_LED_GPIO_PINS;

static uint8_t led_gpio_idx = 0;

// TODO: validate PWM output frequency if user-defined

/* GCLK frequency = MBI_PWM_COUNTER_FREQUENCY / MBI_PWM_PERIOD */
static PWMConfig gclk_pwm_config = {
    .frequency = MBI_PWM_COUNTER_FREQUENCY,
    .period    = MBI_PWM_PERIOD,
};

typedef struct {
#if (MBI_LED_TYPE == MBI_LED_TYPE_RGB)
    uint16_t r;
    uint16_t g;
    uint16_t b;
#elif (MBI_LED_TYPE == MBI_LED_TYPE_MONO)
    uint16_t v;
#endif
} mbi_led_t;

// mbi_leds[0]: back buffer
// mbi_leds[1]: front buffer (sent to MBI)
static mbi_led_t mbi_leds[2][MBI_LED_COUNT];

void mbi_flush_isr(void) {
    /* disable ROW/COL pins */
    for (int i = 0; i < MBI_NUM_LED_GPIO_PINS; i++) {
#if (MBI_LED_GPIO_ACTIVE_HL == LOW)
        writePinHigh(g_led_pins[i]);
#elif (MBI_LED_GPIO_ACTIVE_HL == HIGH)
        writePinLow(g_led_pins[i]);
#endif
    }

    /* latch previous data */
    mbi_send_instruction(MBI_GLOBAL_LATCH);

    /* activate ROW/COL pin */
#if (MBI_LED_GPIO_ACTIVE_HL == LOW)
    writePinLow(g_led_pins[led_gpio_idx]);
#elif (MBI_LED_GPIO_ACTIVE_HL == HIGH)
    writePinHigh(g_led_pins[led_gpio_idx]);
#endif

    led_gpio_idx += 1;
    led_gpio_idx = (led_gpio_idx >= MBI_NUM_LED_GPIO_PINS) ? 0 : led_gpio_idx;

    /* flush data for next ROW/COL */
    writePinLow(MBI_LE_PIN);
    for (int i = MBI_NUM_CHANNELS - 1; i >= 0; i--) {
        uint8_t color_ch;
        uint8_t mbi_ch_idx;
        uint8_t led_idx;
        for (int j = MBI_NUM_DRIVER - 1; j >= 1; j--) {
            color_ch = g_mbi_channels[j][i].color_channel;
            mbi_ch_idx = g_mbi_channels[j][i].color_index;
#if (MBI_LED_DIRECTION == ROW2COL)
            led_idx = g_mbi_led_matrix_co[led_gpio_idx][mbi_ch_idx];
#elif (MBI_LED_DIRECTION == COL2ROW)
            led_idx = g_mbi_led_matrix_co[mbi_ch_idx][led_gpio_idx];
#endif
            switch (color_ch) {
#if (MBI_LED_TYPE == MBI_LED_TYPE_RGB)
                case MBI_RED_CH:
                    mbi_shift_data(mbi_leds[1][led_idx].r, MBI_SHIFT_REG_WIDTH);
                    break;
                case MBI_GREEN_CH:
                    mbi_shift_data(mbi_leds[1][led_idx].g, MBI_SHIFT_REG_WIDTH);
                    break;
                case MBI_BLUE_CH:
                    mbi_shift_data(mbi_leds[1][led_idx].b, MBI_SHIFT_REG_WIDTH);
                    break;
#elif (MBI_LED_TYPE == MBI_LED_TYPE_MONO)
                case MBI_MONO_CH:
                    mbi_shift_data(mbi_leds[1][led_idx].v, MBI_SHIFT_REG_WIDTH);
                    break;
#endif
                case MBI_UNUSED_CH:
                default:
                    mbi_shift_data(0, MBI_SHIFT_REG_WIDTH);
            }
        }
        color_ch = g_mbi_channels[0][i].color_channel;
        mbi_ch_idx = g_mbi_channels[0][i].color_index;
#if (MBI_LED_DIRECTION == ROW2COL)
        led_idx = g_mbi_led_matrix_co[led_gpio_idx][mbi_ch_idx];
#elif (MBI_LED_DIRECTION == COL2ROW)
        led_idx = g_mbi_led_matrix_co[mbi_ch_idx][led_gpio_idx];
#endif
        switch (color_ch) {
#if (MBI_LED_TYPE == MBI_LED_TYPE_RGB)
            case MBI_RED_CH:
                mbi_shift_data_instr(mbi_leds[1][led_idx].r, MBI_SHIFT_REG_WIDTH, MBI_DATA_LATCH);
                break;
            case MBI_GREEN_CH:
                mbi_shift_data_instr(mbi_leds[1][led_idx].g, MBI_SHIFT_REG_WIDTH, MBI_DATA_LATCH);
                break;
            case MBI_BLUE_CH:
                mbi_shift_data_instr(mbi_leds[1][led_idx].b, MBI_SHIFT_REG_WIDTH, MBI_DATA_LATCH);
                break;
#elif (MBI_LED_TYPE == MBI_LED_TYPE_MONO)
            case MBI_MONO_CH:
                mbi_shift_data_instr(mbi_leds[1][led_idx].v, MBI_SHIFT_REG_WIDTH, MBI_DATA_LATCH);
                break;
#endif
            case MBI_UNUSED_CH:
            default:
                mbi_shift_data_instr(0, MBI_SHIFT_REG_WIDTH, MBI_DATA_LATCH);
        }
    }
    writePinLow(MBI_SDI_PIN);
    writePinLow(MBI_DCLK_PIN);
}

static void mbi_gpt_flush_isr(GPTDriver *gptp) {
    mbi_flush_isr();
}

/* ROW/COL update frequency = MBI_TIMER_COUNTER_FREQUENCY / MBI_TIMER_PERIOD */
static GPTConfig mbi_timer_config = {
    .frequency = MBI_TIMER_COUNTER_FREQUENCY,
    .callback  = mbi_gpt_flush_isr,
};

/* user-defined overridable functions */
__attribute__((weak)) void mbi_init_pins(void);
__attribute__((weak)) void mbi_init_config(void);
__attribute__((weak)) void mbi_init_timers(void);

/* Send 'instr' number of DCLK pulses while LE is asserted high. */
void mbi_send_instruction(uint8_t instr) {
    writePinLow(MBI_LE_PIN);
    mbi_io_wait;
    writePinHigh(MBI_LE_PIN);
    while (instr-- > 0) {
        mbi_io_wait;
        writePinLow(MBI_DCLK_PIN);
        mbi_io_wait;
        writePinHigh(MBI_DCLK_PIN);
    }
    writePinLow(MBI_LE_PIN);
}

/* Transmit `shift_amount` bits of `data` to shift-register. */
void mbi_shift_data(uint16_t data, uint8_t shift_amount) {
    while (shift_amount-- > 0) {
        mbi_io_wait;
        writePinLow(MBI_DCLK_PIN);
        writePin(MBI_SDI_PIN, data & MBI_SHIFT_REG_MSB_MASK);
        mbi_io_wait;
        writePinHigh(MBI_DCLK_PIN);
        data = data << 1;
    }
}

/* Transmit `shift_amount` bits of `data` to shift-register, and assert LE for
 * the last `instr` number of DCLK pulses.
 *
 * Note: Assumes `instr` is less than `shift_amount`.
 */
void mbi_shift_data_instr(uint16_t data, uint8_t shift_amount, uint8_t instr) {
    if (instr < shift_amount) {
        writePinLow(MBI_LE_PIN);
        mbi_shift_data(data, shift_amount - instr);
        data = data << (shift_amount - instr);
        writePinHigh(MBI_LE_PIN);
        mbi_shift_data(data, instr);
        writePinLow(MBI_LE_PIN);
    }
}

#if defined(MBI_WRITE_CONFIGURATION) && defined(MBI_ENABLE_WRITE_CONFIGURATION)
/* Write `val` to each MBI configuration register. */
void mbi_write_configuration(uint16_t val) {
    mbi_send_instruction(MBI_ENABLE_WRITE_CONFIGURATION);
    int i = 0;
    for (; i < MBI_NUM_DRIVER - 1; i++) {
        mbi_shift_data(val, MBI_SHIFT_REG_WIDTH);
    }
    if (i < MBI_NUM_DRIVER) {
        mbi_shift_data_instr(val, MBI_SHIFT_REG_WIDTH, MBI_WRITE_CONFIGURATION);
    }
}
#endif

/* Configure and initialize MBI and LED pins */
__attribute__((weak)) void mbi_init_pins(void) {
    setPinOutput(MBI_DCLK_PIN);
    setPinOutput(MBI_GCLK_PIN);
    setPinOutput(MBI_LE_PIN);
    setPinOutput(MBI_SDI_PIN);
    palSetLineMode(MBI_DCLK_PIN, MBI_DCLK_OUTPUT_MODE);
    palSetLineMode(MBI_GCLK_PIN, MBI_GCLK_OUTPUT_MODE);
    palSetLineMode(MBI_LE_PIN, MBI_LE_OUTPUT_MODE);
    palSetLineMode(MBI_SDI_PIN, MBI_SDI_OUTPUT_MODE);
    writePinHigh(MBI_DCLK_PIN);
    writePinHigh(MBI_GCLK_PIN);
    writePinHigh(MBI_LE_PIN);
    writePinHigh(MBI_SDI_PIN);

    /* setup LED ROW/COL pins*/
    for (int i = 0; i < MBI_NUM_LED_GPIO_PINS; i++) {
        setPinOutput(g_led_pins[i]);
        palSetLineMode(g_led_pins[i], MBI_LED_GPIO_OUTPUT_MODE);
#if (MBI_LED_GPIO_ACTIVE_HL == LOW)
        writePinHigh(g_led_pins[i]);
#elif (MBI_LED_GPIO_ACTIVE_HL == HIGH)
        writePinLow(g_led_pins[i]);
#endif
    }

    /* Enable power to MBI(s) if managed by MCU */
#ifdef MBI_POWER_ENABLE_PIN
    setPinOutput(MBI_POWER_ENABLE_PIN);
    palSetLineMode(MBI_POWER_ENABLE_PIN, MBI_POWER_OUTPUT_MODE);
#    if (MBI_POWER_ACTIVE_HL == HIGH)
    writePinHigh(MBI_POWER_ENABLE_PIN);
#    elif (MBI_POWER_ACTIVE_HL == LOW)
    writePinLow(MBI_POWER_ENABLE_PIN);
#    endif
#endif
}

__attribute__((weak)) void mbi_init_config(void) {
#if defined(MBI_WRITE_CONFIGURATION) && defined(MBI_ENABLE_WRITE_CONFIGURATION) && defined(MBI_CONFIGURATION)
    mbi_write_configuration(MBI_CONFIGURATION);
#endif
}

__attribute__((weak)) void mbi_init_timers(void) {
    /* Configure PWM (for GCLK pin) */
    gclk_pwm_config.channels[MBI_PWM_CHANNEL].mode = MBI_PWM_OUTPUT_MODE;
    pwmStart(&MBI_PWM_DRIVER, &gclk_pwm_config);

    /* Start PWM and Timer */
    pwmEnableChannel(&MBI_PWM_DRIVER, MBI_PWM_CHANNEL, PWM_FRACTION_TO_WIDTH(&MBI_PWM_DRIVER, 2, 1));
    gptStart(&MBI_TIMER_DRIVER, &mbi_timer_config);
    if (MBI_TIMER_DRIVER.state == GPT_READY)
        gptStartContinuous(&MBI_TIMER_DRIVER, MBI_TIMER_PERIOD);
}

__attribute__((weak)) void mbi_init(void) {
    /* Configure pins */
    mbi_init_pins();

    /* Configure MBI */
    mbi_init_config();

    /* Start/configure GCLK PWM and timer */
    mbi_init_timers();
}

#if (MBI_LED_TYPE == MBI_LED_TYPE_RGB)
void mbi_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    // Lazy scaling
    mbi_leds[0][index].r = red << (MBI_SHIFT_REG_WIDTH - 8);
    mbi_leds[0][index].g = green << (MBI_SHIFT_REG_WIDTH - 8);
    mbi_leds[0][index].b = blue << (MBI_SHIFT_REG_WIDTH - 8);
}

void mbi_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < MBI_LED_COUNT; i++) {
        mbi_set_color(i, red, green, blue);
    }
}

#elif (MBI_LED_TYPE == MBI_LED_TYPE_MONO)
void mbi_set_value(int index, uint8_t value) {
    // Lazy scaling
    mbi_leds[0][index].v = value << (MBI_SHIFT_REG_WIDTH - 8);
}

void mbi_set_value_all(uint8_t value) {
    for (int i = 0; i < MBI_LED_COUNT; i++) {
        mbi_set_value(i, value);
    }
}
#endif

void mbi_flush(void) {
    memcpy(&mbi_leds[1], &mbi_leds[0], MBI_LED_COUNT * sizeof(mbi_led_t));
}
