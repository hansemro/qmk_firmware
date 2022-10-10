// Copyright (c) 2022 Hansem Ro <hansemro@outlook.com
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

extern bool mbia043_initialized;
extern bool mbia043_is_on;
extern bool mbia043_gclk_enabled;

#define MBIA_NUM_CHANNELS 16
#define MBIA_SHIFT_REGISTER_WIDTH 10 // bits
#define MBIA_NUM_CASCADE 3

#ifndef MBIA_LE_PIN
    #error "MBIA_LE_PIN is not defined"
#endif

#ifndef MBIA_SDI_PIN
    #error "MBIA_SDI_PIN is not defined"
#endif

#ifndef MBIA_SDO_PIN
    #error "MBIA_SDO_PIN is not defined"
#endif

#ifndef MBIA_DCLK_PIN
    #error "MBIA_DCLK_PIN is not defined"
#endif

#ifndef MBIA_GCLK_PIN
    #error "MBIA_GCLK_PIN is not defined"
#endif

// Move data in shift register to buffers
#define MBIA043_INSTR_DATA_LATCH 1
// Move data in shift register to comparators
#define MBIA043_INSTR_OVERALL_LATCH 2

void mbia043_init(void);

void mbia043_send_instruction(uint32_t instr);
void mbia043_shift_data(uint16_t value_be, uint16_t shift_amount);
// for testing
uint16_t mbia043_shift_recv(uint16_t value_be, uint16_t shift_amount);

#ifdef RGB_MATRIX_ENABLE

//#define MATRIX_COLS (MBIA_NUM_CHANNELS)
#define MATRIX_ROWS 8
#define RGB_MATRIX_LED_FLUSH_LIMIT 16

typedef struct {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} mbia043_led_t;

extern mbia043_led_t mbia043_rgb_matrix[MATRIX_ROWS][MBIA_NUM_CHANNELS];
extern bool mbia043_needs_flush;
extern uint8_t mbia043_row;

void _mbia043_reset(void);
void _mbia043_deactivate_row_pins(void);
void mbia043_update_RGB_buffers(void);
void mbia043_flush(void);
void mbia043_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
void mbia043_set_color_all(uint8_t red, uint8_t green, uint8_t blue);

#endif
