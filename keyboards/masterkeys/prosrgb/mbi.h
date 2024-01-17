/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#pragma once

#include <stdint.h>

/* number of output channels of a single MBI */
#ifndef MBI_NUM_CHANNELS
#    error "MBI_NUM_CHANNELS is not defined"
#endif

/* width of a single MBI's shift register in bits */
#ifndef MBI_SHIFT_REG_WIDTH
#    error "MBI_SHIFT_REG_WIDTH is not defined"
#else
#    define MBI_SHIFT_REG_OFFSET (16 - MBI_SHIFT_REG_WIDTH)
#endif

/* number of MBI(s) */
#ifndef MBI_NUM_CASCADE
#    error "MBI_NUM_CASCADE is not defined"
#endif

/* MBI PINS */
#ifndef MBI_LE_PIN
#    error "MBI_LE_PIN is not defined"
#endif

#ifndef MBI_SDI_PIN
#    error "MBI_SDI_PIN is not defined"
#endif

#ifndef MBI_SDO_PIN
#    error "MBI_SDO_PIN is not defined"
#endif

#ifndef MBI_DCLK_PIN
#    error "MBI_DCLK_PIN is not defined"
#endif

#ifndef MBI_GCLK_PIN
#    error "MBI_GCLK_PIN is not defined"
#endif

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

/* FUNCTIONS */

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

/* Find length of shift-register by clearing shift-register with 0s, writing
 * with 1s, and checking how many DCLK pulses until a 1 is detected.
 *
 * Returns a length of MBI_NUM_CASCADE * MBI_SHIFT_REG_WIDTH when ready.
 */
int mbi_get_shift_register_length(void);
