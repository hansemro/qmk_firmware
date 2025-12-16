// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// Copyright 2025 HorrorTroll <https://github.com/HorrorTroll>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <string.h>
#include "gpio.h"
#include "my9373_test.h"

#ifdef SPLIT_KEYBOARD
#    error "Split keyboard not supported"
#endif

pin_t g_my937x_led_pins[MY937X_NUM_LED_GPIO_PINS] = MY937X_LED_GPIO_PINS;

static uint8_t led_gpio_idx = 0;

// TODO: validate PWM output frequency if user-defined

/* GCLK frequency = MY937X_PWM_COUNTER_FREQUENCY / MY937X_PWM_PERIOD */
static PWMConfig gck_pwm_config = {
    .frequency = MY937X_PWM_COUNTER_FREQUENCY,
    .period    = MY937X_PWM_PERIOD,
};

typedef struct {
#if (MY937X_LED_TYPE == MY937X_LED_TYPE_RGB)
    uint16_t r;
    uint16_t g;
    uint16_t b;
#elif (MY937X_LED_TYPE == MY937X_LED_TYPE_MONO)
    uint16_t v;
#endif
} my937x_led_t;

// my937x_leds[0]: back buffer
// my937x_leds[1]: front buffer (sent to MY937X)
static my937x_led_t my937x_leds[2][MY937X_LED_COUNT];

uint16_t scan_data[MY937X_NUM_LED_GPIO_PINS][MY937X_NUM_DRIVER][MY937X_NUM_CHANNELS];

//static void my937x_gpt_flush_isr(GPTDriver *gptp) {
//    my937x_flush_isr();
//}

/* ROW/COL update frequency = MY937X_GPT_COUNTER_FREQUENCY / MY937X_GPT_PERIOD */
//static GPTConfig my937x_gpt_config = {
//    .frequency = MY937X_GPT_COUNTER_FREQUENCY,
//    .callback  = my937x_gpt_flush_isr,
//};

void inline my937x_sdi_bit(uint8_t bit) {
    if (bit) {
        gpio_write_pin_high(MY937X_SDI_PIN);
    } else {
        gpio_write_pin_low(MY937X_SDI_PIN);
    }

    my937x_io_wait;
    gpio_write_pin_high(MY937X_DCK_PIN);
    my937x_io_wait;
    gpio_write_pin_low(MY937X_DCK_PIN);
}

void inline my937x_send_32bits(uint32_t word) {
  for (int i = 31; i >= 0; i--) {
    my937x_sdi_bit((word >> i) & 1);
  }
}

void inline my937x_send_16bits(uint16_t word) {
  for (int i = 15; i >= 0; i--) {
    my937x_sdi_bit((word >> i) & 1);
  }
}

/* Ensure DCK is returned to idle low at function entry/exit. */
void inline my937x_dck_instruction(uint8_t instr) {
    while (instr-- > 0) {
        my937x_io_wait;
        gpio_write_pin_high(MY937X_DCK_PIN);
        my937x_io_wait;
        gpio_write_pin_low(MY937X_DCK_PIN);
    }
}

/* Initial instruction: LAT HIGH while sending 5 DCK pulses (frame start/end) */
void inline my937x_frame_instruction(void) {
    gpio_write_pin_high(MY937X_LAT_PIN);
    my937x_dck_instruction(5);
    gpio_write_pin_low(MY937X_LAT_PIN);
}

/* Write 32-bit command data to each driver (32 bits × M), then single LAT pulse */
void my937x_write_command_data(void) {
    for (int i = 0; i < MY937X_NUM_DRIVER; i++) {
        my937x_send_32bits(MY937X_COMMAND_DATA);
    }

    gpio_write_pin_high(MY937X_LAT_PIN);
    my937x_io_wait;
    gpio_write_pin_low(MY937X_LAT_PIN);
    my937x_io_wait;
}

//void inline my937x_gck_segment_config(uint32_t cmd, uint16_t *segment_pulses, uint16_t *segments_per_frame) {
//    uint8_t refresh_mode = (cmd >> 8) & 0x03; // CMD[9:8]
//    bool double_edge = (cmd >> 19) & 0x01;    // CMD[19]
//
//    // GCK pulses per segment (includes final TOff rising edge)
//    static const uint16_t gck_pulses_by_mode[] = { 513, 257, 129, 65 };
//
//    if (refresh_mode > 3) refresh_mode = 0;
//
//    *segment_pulses = gck_pulses_by_mode[refresh_mode];
//
//    if (double_edge) {
//        static const uint16_t segments_double[] = { 64, 128, 256, 512 };   // For 15-bit double-edge
//        *segments_per_frame = segments_double[refresh_mode];
//    } else {
//        static const uint16_t segments_single[] = { 128, 256, 512, 1024 }; // For 16-bit single-edge
//        *segments_per_frame = segments_single[refresh_mode];
//    }
//}

void inline extract_scan_count(uint32_t cmd, uint8_t *scan_count) {
    uint8_t scan_bits = (cmd >> 10) & 0x0F;

    // Scan mode 0000 = static mode = 1 row
    // 0001 = 1/2 scan (2 rows), ..., 1111 = 1/16 scan (16 rows)
    if (scan_bits == 0) {
        *scan_count = 1;
    } else {
        *scan_count = scan_bits + 1; // e.g., 0001 = 2 rows
    }
}

void inline my937x_write_scan_segment(uint16_t scan_line[MY937X_NUM_DRIVER][MY937X_NUM_CHANNELS]) {
    for (int chip = MY937X_NUM_DRIVER - 1; chip >= 0; chip--) {
        for (int ch = MY937X_NUM_CHANNELS - 1; ch >= 0; ch--) {
            my937x_send_16bits(scan_line[chip][ch]);
        }
    }

    gpio_write_pin_high(MY937X_LAT_PIN);
    my937x_io_wait;
    gpio_write_pin_low(MY937X_LAT_PIN);
    my937x_io_wait;
}

void hue_to_rgb(uint16_t hue, uint16_t *r, uint16_t *g, uint16_t *b) {
    uint8_t region = hue / 255;
    uint16_t remainder = (hue % 255) * 255;

    uint16_t p = 0;
    uint16_t q = 65535 - remainder;
    uint16_t t = remainder;

    switch (region) {
        case 0: *r = 65535; *g = t;     *b = p;     break;
        case 1: *r = q;     *g = 65535; *b = p;     break;
        case 2: *r = p;     *g = 65535; *b = t;     break;
        case 3: *r = p;     *g = q;     *b = 65535; break;
        case 4: *r = t;     *g = p;     *b = 65535; break;
        case 5: default:
                *r = 65535; *g = p;     *b = q;     break;
    }

    // Apply 8-bit banding here (reduce precision)
//    *r = (*r >> 8) << 8;
//    *g = (*g >> 8) << 8;
//    *b = (*b >> 8) << 8;
}

void update_color_wave(uint16_t phase) {
    for (int i = 0; i < MY937X_LED_COUNT; i++) {
        // full-resolution wave (65536 range)
        uint16_t hue = ((uint16_t)(phase + (MY937X_LED_COUNT * (1536 / MY937X_NUM_CHANNELS)))) % 1536;

        uint16_t r, g, b;
        hue_to_rgb(hue, &r, &g, &b);

        my937x_leds[0][i].r = r;
        my937x_leds[0][i].g = g;
        my937x_leds[0][i].b = b;
    }
}

void my937x_flush(void) {
    memcpy(&my937x_leds[1], &my937x_leds[0], MY937X_LED_COUNT * sizeof(my937x_led_t));
}

/* Configure and initialize MY937X and LED pins */
__attribute__((weak)) void my937x_init_pins(void) {
    palSetLineMode(MY937X_DCK_PIN, MY937X_DCK_OUTPUT_MODE);
    palSetLineMode(MY937X_GCK_PIN, MY937X_GCK_OUTPUT_MODE);
    palSetLineMode(MY937X_LAT_PIN, MY937X_LAT_OUTPUT_MODE);
    palSetLineMode(MY937X_SDI_PIN, MY937X_SDI_OUTPUT_MODE);

    gpio_write_pin_low(MY937X_DCK_PIN);
    gpio_write_pin_low(MY937X_GCK_PIN);
    gpio_write_pin_low(MY937X_LAT_PIN);
    gpio_write_pin_low(MY937X_SDI_PIN);

    /* Setup LED ROW/COL pins*/
    for (int i = 0; i < MY937X_NUM_LED_GPIO_PINS; i++) {
        palSetLineMode(g_my937x_led_pins[i], MY937X_LED_GPIO_OUTPUT_MODE);
    }

    /* Enable power to MY937X if managed by MCU */
#ifdef MY937X_POWER_ENABLE_PIN
    palSetLineMode(MY937X_POWER_ENABLE_PIN, MY937X_POWER_OUTPUT_MODE);
#    if (MY937X_POWER_ACTIVE_STATE == ACTIVE_HIGH)
    gpio_write_pin_high(MY937X_POWER_ENABLE_PIN);
#    else
    gpio_write_pin_low(MY937X_POWER_ENABLE_PIN);
#    endif
#endif
}

__attribute__((weak)) void my937x_init_pwm(void) {
    /* Configure PWM (for GCK pin) */
    gck_pwm_config.channels[MY937X_PWM_CHANNEL - 1].mode = MY937X_PWM_OUTPUT_MODE;
    pwmStart(&MY937X_PWM_DRIVER, &gck_pwm_config);

    /* Start PWM channel for GCK (50% duty or configured width) */
    pwmEnableChannel(&MY937X_PWM_DRIVER, MY937X_PWM_CHANNEL - 1, PWM_PERCENTAGE_TO_WIDTH(&MY937X_PWM_DRIVER, 5000));
}

//__attribute__((weak)) void my937x_init_gpt(void) {
//    gptStart(&MY937X_GPT_DRIVER, &my937x_gpt_config);
//    if (MY937X_GPT_DRIVER.state == GPT_READY)
//        gptStartContinuous(&MY937X_GPT_DRIVER, MY937X_GPT_PERIOD);
//}

void keyboard_post_init_user(void) {
    /* Configure pins */
    my937x_init_pins();

    /* Start/configure GPT */
    //my937x_init_gpt();

    //inline void my937x_flush_isr(void) {
    while (true) {
        uint16_t frame_phase = 0;

        my937x_frame_instruction();
        my937x_init_pwm();
        my937x_write_command_data();

        // Update once per frame
        update_color_wave(frame_phase);

        // Send only one scan per segment_tick
        uint8_t scan_count;
        //uint16_t pulses, total_segments;
        //my937x_gck_segment_config(MY937X_COMMAND_DATA, &pulses, &total_segments);
        extract_scan_count(MY937X_COMMAND_DATA, &scan_count);

        /* Disable ROW/COL pins */
        for (int i = 0; i < scan_count; i++) {
    #if (MY937X_LED_GPIO_ACTIVE_STATE == ACTIVE_LOW)
            gpio_write_pin_high(g_my937x_led_pins[i]);
    #else
            gpio_write_pin_low(g_my937x_led_pins[i]);
    #endif
    
            my937x_write_scan_segment(scan_data[i]);
        }

        /* Enable ROW/COL pins */
    #if (MY937X_LED_GPIO_ACTIVE_STATE == ACTIVE_LOW)
        gpio_write_pin_low(g_my937x_led_pins[led_gpio_idx]);
    #else
        gpio_write_pin_high(g_my937x_led_pins[led_gpio_idx]);
    #endif

        led_gpio_idx += 1;
        led_gpio_idx = (led_gpio_idx >= scan_count) ? 0 : led_gpio_idx;

        /* Flush data for next ROW/COL */
        for (int i = MY937X_NUM_CHANNELS - 1; i >= 0; i--) {
            uint8_t color_ch;
            uint8_t my937x_ch_idx;
            uint8_t led_idx;
            uint16_t color_val;
            for (int j = MY937X_NUM_DRIVER - 1; j >= 0; j--) {
                color_ch = g_my937x_channels[j][i].color_channel;
                my937x_ch_idx = g_my937x_channels[j][i].color_index;
    #if (MY937X_LED_DIRECTION == ROW2COL)
                led_idx = g_my937x_led_matrix_co[led_gpio_idx][my937x_ch_idx];
    #elif (MY937X_LED_DIRECTION == COL2ROW)
                led_idx = g_my937x_led_matrix_co[my937x_ch_idx][led_gpio_idx];
    #endif
                switch (color_ch) {
    #if (MY937X_LED_TYPE == MY937X_LED_TYPE_RGB)
                    case MY937X_RED_CH:
                        color_val = my937x_leds[1][led_idx].r;
                        break;
                    case MY937X_GREEN_CH:
                        color_val = my937x_leds[1][led_idx].g;
                        break;
                    case MY937X_BLUE_CH:
                        color_val = my937x_leds[1][led_idx].b;
                        break;
    #elif (MY937X_LED_TYPE == MY937X_LED_TYPE_MONO)
                    case MY937X_MONO_CH:
                        color_val = my937x_leds[1][led_idx].v;
                        break;
    #endif
                    case MY937X_UNUSED_CH:
                    default:
                        color_val = 0;
                }

                scan_data[led_gpio_idx][j][i] = color_val;
            }
        }

        // Advance to next segment
        frame_phase = (frame_phase + 4);
        my937x_flush();
    }
}
