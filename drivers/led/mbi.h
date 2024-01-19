/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
 */

#pragma once

/*
 * Bit-banged driver for Macroblock MBIA043/MBIA045/MBI5042/MBI5043 (LED
 * drivers with proprietary SPI-like interface) using PWM for GCLK and
 * General Purpose Timer (GPT) for flushing color data.
 */

#include <stdint.h>

/* number of output channels of a single MBI */
#ifndef MBI_NUM_CHANNELS
#    define 16
#endif

/* width of a single MBI's shift register in bits */
#ifndef MBI_SHIFT_REG_WIDTH
#    error "MBI_SHIFT_REG_WIDTH is not defined"
#else
#    if (MBI_SHIFT_REG_WIDTH >= 8) && (MBI_SHIFT_REG_WIDTH <= 16)
/* used for sampling most significant bit */
#        define MBI_SHIFT_REG_MSB_MASK (1U << (MBI_SHIFT_REG_WIDTH - 1))
#    else
#        error "MBI_SHIFT_REG_WIDTH must be between 8 and 16 (bits)"
#    endif
#endif

/* number of MBI(s) */
#ifndef MBI_NUM_DRIVER
#    error "MBI_NUM_DRIVER is not defined"
#endif

/* RGB or Grayscale (mono) LEDs */
#ifdef MBI_LED_TYPE
#    if (MBI_LED_TYPE != RGB) && (MBI_LED_TYPE != MONO)
#        error "MBI_LED_TYPE is not defined to either RGB or MONO"
#    endif
#else
#    error "MBI_LED_TYPE is not defined to either RGB or MONO"
#endif

/* LED matrix direction: ROW2COL or COL2ROW */
#ifndef MBI_LED_DIRECTION
#    error "MBI_LED_DIRECTION is not defined to either ROW2COL or COL2ROW"
#else
#    if (MBI_LED_DIRECTION == ROW2COL)
#        define MBI_LED_COUNT (MBI_NUM_CHANNELS * MATRIX_ROWS)
#    elif (MBI_LED_DIRECTION == COL2ROW)
#        define MBI_LED_COUNT (MBI_NUM_CHANNELS * MATRIX_COLS)
#    else
#        error "MBI_LED_DIRECTION is not defined to either ROW2COL or COL2ROW"
#    endif
#endif

/* ROW/COL GPIO pins connected to anode of LEDs */
#ifndef MBI_LED_GPIO_PINS
#    error "MBI_LED_GPIO_PINS"
#endif

#ifndef MBI_LED_GPIO_OUTPUT_MODE
#    error "MBI_LED_GPIO_OUTPUT_MODE is not defined"
#endif

#ifndef MBI_PWM_DRIVER
#    error "MBI_PWM_DRIVER is not defined"
#endif

#ifndef MBI_PWM_CHANNEL
#    error "MBI_PWM_CHANNEL is not defined"
#endif

#ifndef MBI_PWM_OUTPUT_MODE
#    define MBI_PWM_OUTPUT_MODE PWM_OUTPUT_ACTIVE_LOW
#endif

#if !defined(MBI_PWM_COUNTER_FREQUENCY) && !defined(MBI_PWM_PERIOD)
/* PWM counter frequency in Hz */
#    define MBI_PWM_COUNTER_FREQUENCY 8000000UL
/* PWM period in counter ticks */
#    define MBI_PWM_PERIOD 2UL
#else
#    ifndef MBI_PWM_COUNTER_FREQUENCY
#        error "MBI_PWM_COUNTER_FREQUENCY is not defined"
#    endif
#    ifndef MBI_PWM_PERIOD
#        error "MBI_PWM_PERIOD is not defined"
#    endif
#endif

#ifndef MBI_TIMER_DRIVER
#    error "MBI_TIMER_DRIVER is not defined"
#endif

#if !defined(MBI_TIMER_COUNTER_FREQUENCY) && !defined(MBI_TIMER_PERIOD)
/* Timer counter frequency in Hz */
#    define MBI_TIMER_COUNTER_FREQUENCY 2000UL
/* Timer period in counter ticks */
#    define MBI_TIMER_PERIOD 2UL
#else
#    ifndef MBI_TIMER_COUNTER_FREQUENCY
#        error "MBI_TIMER_COUNTER_FREQUENCY is not defined"
#    endif
#    ifndef MBI_TIMER_PERIOD
#        error "MBI_TIMER_PERIOD is not defined"
#    endif
#endif

/* MBI PINS */

#ifndef MBI_LE_PIN
#    error "MBI_LE_PIN is not defined"
#endif

#ifndef MBI_LE_OUTPUT_MODE
#    error "MBI_LE_OUTPUT_MODE is not defined"
#endif

#ifndef MBI_SDI_PIN
#    error "MBI_SDI_PIN is not defined"
#endif

#ifndef MBI_SDI_OUTPUT_MODE
#    error "MBI_SDI_OUTPUT_MODE is not defined"
#endif

#ifndef MBI_SDO_PIN
#    error "MBI_SDO_PIN is not defined"
#endif

#ifndef MBI_SDO_INPUT_MODE
#    error "MBI_SDO_INPUT_MODE is not defined"
#endif

#ifndef MBI_DCLK_PIN
#    error "MBI_DCLK_PIN is not defined"
#endif

#ifndef MBI_DCLK_OUTPUT_MODE
#    error "MBI_DCLK_OUTPUT_MODE is not defined"
#endif

#ifndef MBI_GCLK_PIN
#    error "MBI_GCLK_PIN is not defined"
#endif

#ifndef MBI_GCLK_OUTPUT_MODE
#    error "MBI_GCLK_OUTPUT_MODE is not defined"
#endif

/* Some keyboards have a pin to enable power to MBI */
#ifdef MBI_POWER_ENABLE_PIN
#    if !defined(MBI_POWER_ACTIVE_HL) && (MBI_POWER_ACTIVE_HL != HIGH) && (MBI_POWER_ACTIVE_HL != LOW)
#        error "MBI_POWER_ACTIVE_HL must be defined to either HIGH or LOW"
#    endif
#    ifndef MBI_POWER_OUTPUT_MODE
#        error "MBI_POWER_OUTPUT_MODE is not defined"
#    endif
#endif


/* Insert nop delays to operate under 25MHz */
#ifndef MBI_NOPS
#    if defined(__AVR__)
#        define MBI_NOPS 0 // AVR at 16 MHz already spends 62.5 ns per clock, so no extra delay is needed
#    elif defined(PROTOCOL_CHIBIOS)
#        include "hal.h"
#        include "chibios_config.h"
#        if defined(HT32F165x) || defined(HT32F523x2) || defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F3XX) || defined(STM32F4XX) || defined(STM32L0XX) || defined(GD32VF103) || defined(MCU_RP)
#            define MBI_NOPS (40 / (1000000000L / (CPU_CLOCK / 3))) // This calculates how many loops of 3 nops to run to delay 40 ns
#        else
#            error MBI_NOPS configuration required
#            define MBI_NOPS 0 // this just pleases the compile so the above error is easier to spot
#        endif
#    endif
#endif

#define mbi_io_wait                                 \
    do {                                        \
        for (int i = 0; i < MBI_NOPS; i++) { \
            __asm__ volatile("nop\n\t"          \
                             "nop\n\t"          \
                             "nop\n\t");        \
        }                                       \
    } while (0)

/* INSTRUCTIONS: */

/* Move data in shift-register to a single buffer */
#ifndef MBI_DATA_LATCH
#    error "MBI_DATA_LATCH is not defined"
#endif

/* Move data in all buffers to comparators */
#ifndef MBI_GLOBAL_LATCH
#    error "MBI_GLOBAL_LATCH is not defined"
#endif

/* Move configuration register to shift-register */
#ifndef MBI_READ_CONFIGURATION
#    error "MBI_READ_CONFIGURATION is not defined"
#endif

/* If previous instruction was MBI_ENABLE_WRITE_CONFIGURATION, then move
 * data in shift-register to configuration register
 */
#ifndef MBI_WRITE_CONFIGURATION
#    error "MBI_WRITE_CONFIGURATION is not defined"
#endif

/* Enable MBI_WRITE_CONFIGURATION */
#ifndef MBI_ENABLE_WRITE_CONFIGURATION
#    error "MBI_ENABLE_WRITE_CONFIGURATION is not defined"
#endif

/* Send 'instr' number of DCLK pulses while LE is asserted high. */
void mbi_send_instruction(int instr);

/* Transmit data to shift-register with shift_amount number of DCLK pulses.
 *
 * Note: Transmission begins with MSB at data[15].
 */
void mbi_shift_data(uint16_t data, int shift_amount);

/* Transmit data to shift-register with shift_amount number of DCLK pulses,
 * and assert LE for the last instr number of DCLK pulses.
 *
 * Note: Assumes instr is less than shift_amount.
 * Note: Transmission begins with MSB at data[15].
 */
void mbi_shift_data_instr(uint16_t data, int shift_amount, int instr);

/* Transmit data to shift-register with shift_amount number of DCLK pulses,
 * and read shift_amount bits of data from (last-in-cascade) shift-register.
 *
 * Note: Transmission begins with MSB at data[15].
 */
uint16_t mbi_shift_recv(uint16_t data, int shift_amount);

/* Write val to each MBI's configuration registers.
 *
 * Note: order of array follows order from SDO to SDI pins.
 */
void mbi_write_configuration(uint16_t val);

/* initialize mbi driver(s) */
void mbi_init(void);
#if (MBI_LED_TYPE == RGB)
void mbi_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
void mbi_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
#elif (MBI_LED_TYPE == MONO)
void mbi_set_value(int index, uint8_t value);
void mbi_set_value_all(uint8_t value);
#endif
void mbi_flush(void);

#define MBI_UNUSED_CH 0
#define MBI_MONO_CH 1
#define MBI_RED_CH 1
#define MBI_GREEN_CH 2
#define MBI_BLUE_CH 3

typedef struct {
    uint8_t color_channel;
    uint8_t color_index; // ROW/COL index
} mbi_channel_t;

/* Output channel to color channel mapping */
extern const mbi_channel_t g_mbi_channels[MBI_NUM_DRIVER][MBI_NUM_CHANNELS];

/* LED Matrix to LED Index */
extern const uint8_t g_mbi_led_matrix_co[MATRIX_ROWS][MATRIX_COLS];
