/* SDPX-License-Identifier: GPL-2.0-or-later
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
 */

#pragma once

#include <stdint.h>

#define MBI5042_NUM_CHANNELS 16
#define MBI5042_SHIFT_REG_WIDTH 16 // bits
#ifndef MBI5042_NUM_CASCADE
#    error "MBI5042_NUM_CASCADE is not defined"
#endif

#ifndef MBI5042_LE_PIN
#    error "MBI5042_LE_PIN is not defined"
#endif

#ifndef MBI5042_SDI_PIN
#    error "MBI5042_SDI_PIN is not defined"
#endif

#ifndef MBI5042_SDO_PIN
#    error "MBI5042_SDO_PIN is not defined"
#endif

#ifndef MBI5042_DCLK_PIN
#    error "MBI5042_DCLK_PIN is not defined"
#endif

#ifndef MBI5042_GCLK_PIN
#    error "MBI5042_GCLK_PIN is not defined"
#endif

/* Pin used to control power supply to MBI(s) */
#ifdef MBI5042_HAS_POWER_PIN
#    ifndef MBI5042_PWRCTRL_PIN
#        error "MBI5042_PWRCTRL_PIN is not defined"
#    endif
#endif

/* INSTRUCTIONS: */
/* Move data in shift-register to a single buffer */
#define MBI5042_DATA_LATCH 1
/* Move data in all buffers to comparators */
#define MBI5042_GLOBAL_LATCH 3
/* Move configuration register to shift-register */
#define MBI5042_READ_CONFIGURATION 5
/* If previous instruction was MBI5042_ENABLE_WRITE_CONFIGURATION, then move
 * data in shift-register to configuration register
 */
#define MBI5042_WRITE_CONFIGURATION 11
/* Enable MBI5042_WRITE_CONFIGURATION */
#define MBI5042_ENABLE_WRITE_CONFIGURATION 15

/* FUNCTIONS */
void mbi5042_init(void);

void     mbi5042_send_instruction(int instr);
void     mbi5042_shift_data(uint16_t data, int shift_amount);
void     mbi5042_shift_data_instr(uint16_t data, int shift_amount, int instr);
uint16_t mbi5042_shift_recv(uint16_t data, int shift_amount);

void mbi5042_read_configuration(uint16_t *dst);
void mbi5042_write_configuration(uint16_t *src);

/* Returns a length of MBI5042_NUM_CASCADE * MBI5042_SHIFT_REG_WIDTH when
 * ready.
 */
int mbi5042_get_shift_register_length(void);
