// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * LED or RGB Matrix driver for Macroblock MBI LED drivers with proprietary
 * SPI-like control interface using PWM for GCLK and General Purpose Timer
 * (GPT) for flushing color data.
 *
 * This driver can support the following MBI families:
 * - MBIA04X
 * - MBI504X
 * - possibly others with the same control interface
 *
 * Although MBI parts may have two or more instructions, this driver will only
 * require the Data Latch and Global Latch instructions. MBI instructions to
 * write to configuration register is made optional for MBI parts without a
 * (documented) configuration register (such as MBIA043).
 *
 * This driver does not pre-define instructions or configuration settings.
 */

#include <stdint.h>
#include "gpio.h"

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

/* MBI Data Loading Types:
 * - Serial: MCU drives serial data to first MBI with serial data output of
 *           one MBI driving input of next MBI
 * - Parallel: MCU drives serial data to each MBI in parallel
 */
#define MBI_LOAD_TYPE_SERIAL 0
#define MBI_LOAD_TYPE_PARALLEL 1

#if (MBI_NUM_DRIVER == 1) && !defined(MBI_LOAD_TYPE)
#    define MBI_LOAD_TYPE MBI_LOAD_TYPE_SERIAL
#elif !defined(MBI_LOAD_TYPE)
#    error "MBI_LOAD_TYPE must be defined to either MBI_LOAD_TYPE_SERIAL or MBI_LOAD_TYPE_PARALLEL"
#endif

/* RGB or Grayscale (mono) LEDs */
#define MBI_LED_TYPE_MONO 0
#define MBI_LED_TYPE_RGB 1

/* Select between Mono or RGB type driver */
#ifdef MBI_LED_TYPE
#    if (MBI_LED_TYPE != MBI_LED_TYPE_RGB) && (MBI_LED_TYPE != MBI_LED_TYPE_MONO)
#        error "MBI_LED_TYPE is not defined to either MBI_LED_TYPE_RGB or MBI_LED_TYPE_MONO"
#    endif
#else
#    error "MBI_LED_TYPE is not defined to either MBI_LED_TYPE_RGB or MBI_LED_TYPE_MONO"
#endif

/* LED matrix directions:
 * - COL2ROW: from MCU-managed column pins to MBI-managed row pins
 * - ROW2COL: from MCU-managed row pins to MBI-managed column pins
 */
#ifndef COL2ROW
#    define COL2ROW 0
#endif
#ifndef ROW2COL
#    define ROW2COL 1
#endif

#ifndef MBI_LED_DIRECTION
#    error "MBI_LED_DIRECTION is not defined to either ROW2COL or COL2ROW"
#endif

/* MCU-managed row/column LED pins */
#ifndef MBI_LED_GPIO_PINS
#    error "MBI_LED_GPIO_PINS is not defined with an array of pins"
#endif

#ifndef MBI_LED_GPIO_OUTPUT_MODE
#    error "MBI_LED_GPIO_OUTPUT_MODE is not defined"
#endif

#ifndef MBI_NUM_LED_GPIO_PINS
#    error "MBI_NUM_LED_GPIO_PINS is not defined"
#else
#    define MBI_LED_COUNT (MBI_NUM_CHANNELS * MBI_NUM_LED_GPIO_PINS)
#endif

#ifndef HIGH
#    define HIGH 1
#endif
#ifndef LOW
#    define LOW 0
#endif

#ifdef MBI_LED_GPIO_ACTIVE_HL
#    if (MBI_LED_GPIO_ACTIVE_HL != HIGH) && (MBI_LED_GPIO_ACTIVE_HL != LOW)
#        error "MBI_LED_GPIO_ACTIVE_HL must be defined to either HIGH or LOW"
#    endif
#else
#    error "MBI_LED_GPIO_ACTIVE_HL is not defined"
#endif

/* PWM driver to use for generating GCLK clock signal */
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

/* GPT timer driver to use for continuous row/column pin cycling and data flushing */
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

#if !defined(MBI_SDI_PIN) && !defined(MBI_SDI_PINS)
#    error "define either MBI_SDI_PIN or MBI_SDI_PINS (for parallel operation)"
#endif

#ifndef MBI_SDI_OUTPUT_MODE
#    error "MBI_SDI_OUTPUT_MODE is not defined"
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

/* (Optional) MCU-managed pin to enable power to MBI */
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
#        if defined(HT32) || defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F3XX) || defined(STM32F4XX) || defined(STM32L0XX) || defined(GD32VF103) || defined(MCU_RP)
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

/* MBI INSTRUCTIONS: */

/* Move data in shift-register to a single buffer */
#ifndef MBI_DATA_LATCH
#    error "MBI_DATA_LATCH is not defined"
#endif

/* Move data in all buffers to comparators */
#ifndef MBI_GLOBAL_LATCH
#    error "MBI_GLOBAL_LATCH is not defined"
#endif

/* If previous instruction was MBI_ENABLE_WRITE_CONFIGURATION, then move
 * data in shift-register to configuration register
 */
#if defined(MBI_WRITE_CONFIGURATION) && !defined(MBI_ENABLE_WRITE_CONFIGURATION)
#    error "MBI_WRITE_CONFIGURATION defined without defining MBI_ENABLE_WRITE_CONFIGURATION"
#endif

/*
 * Configuration setting notes for boards defining MBI_CONFIGURATION:
 * - If "Data Loading" setting exists, select option for <MBI_NUM_CHANNELS> of Data Latch + 1 of Global Latch
 * - If "N-bit PWM Counter Mode" setting exists, keep it consistent with MBI_SHIFT_REG_WIDTH
 */

/* (Optional) Configuration value to set each MBI on initialization */
#if defined(MBI_CONFIGURATION)
#    if !defined(MBI_WRITE_CONFIGURATION) || !defined(MBI_ENABLE_WRITE_CONFIGURATION)
#        error "MBI_WRITE_CONFIGURATION and MBI_ENABLE_WRITE_CONFIGURATION must be defined to set MBI_CONFIGURATION"
#    endif
#endif

/* Send 'instr' number of DCLK pulses while LE is asserted high. */
void mbi_send_instruction(uint8_t instr);

/* Transmit `shift_amount` bits of `data` to shift-register. */
void mbi_shift_data(uint16_t data, uint8_t shift_amount);

/* Transmit `shift_amount` bits of `data` to shift-register, and assert LE for
 * the last `instr` number of DCLK pulses.
 *
 * Note: Assumes `instr` is less than `shift_amount`.
 */
void mbi_shift_data_instr(uint16_t data, uint8_t shift_amount, uint8_t instr);

#if (MBI_LOAD_TYPE == MBI_LOAD_TYPE_PARALLEL)
/* Transmit `shift_amount` bits of `data[n]` to nth driver's shift-register. */
void mbi_parallel_shift_data(const uint16_t data[MBI_NUM_DRIVER], uint8_t shift_amount);

/* Transmit `shift_amount` bits of `data[n]` to nth driver's shift-register,
 * and assert LE for the last `instr` number of DCLK pulses.
 *
 * Note: Assumes `instr` is less than `shift_amount`.
 */
void mbi_parallel_shift_data_instr(const uint16_t data[MBI_NUM_DRIVER], uint8_t shift_amount, uint8_t instr);
#endif

#if defined(MBI_WRITE_CONFIGURATION) && defined(MBI_ENABLE_WRITE_CONFIGURATION)
/* Write val to each MBI configuration register. */
void mbi_write_configuration(uint16_t val);
#endif

/* initialize mbi driver(s) */
void mbi_init(void);
#if (MBI_LED_TYPE == MBI_LED_TYPE_RGB)
/* write RGB color to back buffer at a specific index */
void mbi_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
/* write RGB color to entire back buffer */
void mbi_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
#elif (MBI_LED_TYPE == MBI_LED_TYPE_MONO)
/* write grayscale value to back buffer at a specific index */
void mbi_set_value(int index, uint8_t value);
/* write grayscale value to entire back buffer */
void mbi_set_value_all(uint8_t value);
#endif
/* updates front buffer from back buffer */
void mbi_flush(void);
/* activate row/column pin and update buffers for next row/column */
void mbi_flush_isr(void);

#define MBI_UNUSED_CH 0
#define MBI_MONO_CH 1
#define MBI_RED_CH 1
#define MBI_GREEN_CH 2
#define MBI_BLUE_CH 3

typedef struct {
    uint8_t color_channel;
    uint8_t color_index; // MBI ROW/COL index for the color channel
} mbi_channel_t;

/* Output channel to color channel mapping */
extern const mbi_channel_t g_mbi_channels[MBI_NUM_DRIVER][MBI_NUM_CHANNELS];

/* LED Matrix to LED Index */
#if (MBI_LED_DIRECTION == ROW2COL)
extern const uint8_t g_mbi_led_matrix_co[MBI_NUM_LED_GPIO_PINS][MBI_NUM_CHANNELS];
#elif (MBI_LED_DIRECTION == COL2ROW)
extern const uint8_t g_mbi_led_matrix_co[MBI_NUM_CHANNELS][MBI_NUM_LED_GPIO_PINS];
#endif

/* LED row/column pins */
extern pin_t g_led_pins[MBI_NUM_LED_GPIO_PINS];

#if (MBI_LOAD_TYPE == MBI_LOAD_TYPE_PARALLEL)
extern pin_t g_sdi_pins[MBI_NUM_DRIVER];
#endif
