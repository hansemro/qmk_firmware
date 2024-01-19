// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// SDPX-License-Identifier: GPL-2.0-or-later

#include "hal.h"
#include "gpio.h"
#include "mbi.h"
#include "quantum.h"

static inline void mbi_nop_delay(void) {
    __NOP();
    __NOP();
    __NOP();
}

/* Send 'instr' number of DCLK pulses while LE is asserted high. */
void mbi_send_instruction(int instr) {
    writePinLow(MBI_LE_PIN);
    mbi_nop_delay();
    writePinHigh(MBI_LE_PIN);
    while (instr-- > 0) {
        mbi_nop_delay();
        writePinLow(MBI_DCLK_PIN);
        mbi_nop_delay();
        writePinHigh(MBI_DCLK_PIN);
    }
    writePinLow(MBI_LE_PIN);
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses.
 *
 * Note: Transmission begins with MSB at data[15].
 */
void mbi_shift_data(uint16_t data, int shift_amount) {
    while (shift_amount-- > 0) {
        mbi_nop_delay();
        writePinLow(MBI_DCLK_PIN);
        // set SDI to data[15]
        writePin(MBI_SDI_PIN, data & 0x8000);
        mbi_nop_delay();
        // clock in data
        writePinHigh(MBI_DCLK_PIN);
        data = data << 1;
    }
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses,
 * and assert LE for the last instr number of DCLK pulses.
 *
 * Note: Assumes instr is less than shift_amount.
 * Note: Transmission begins with MSB at data[15].
 */
void mbi_shift_data_instr(uint16_t data, int shift_amount, int instr) {
    if (instr < shift_amount) {
        writePinLow(MBI_LE_PIN);
        mbi_shift_data(data, shift_amount - instr);
        data = data << (shift_amount - instr);
        writePinHigh(MBI_LE_PIN);
        mbi_shift_data(data, instr);
        writePinLow(MBI_LE_PIN);
    }
    return;
}

/* Transmit data to shift-register with shift_amount number of DCLK pulses,
 * and read shift_amount bits of data from (last-in-cascade) shift-register.
 *
 * Note: Transmission begins with MSB at data[15].
 */
uint16_t mbi_shift_recv(uint16_t data, int shift_amount) {
    uint16_t recv = 0;
    while (shift_amount-- > 0) {
        mbi_nop_delay();
        writePinLow(MBI_DCLK_PIN);
        recv = (recv << 1) | readPin(MBI_SDO_PIN);
        // set SDI to data[15]
        writePin(MBI_SDI_PIN, data & 0x8000);
        mbi_nop_delay();
        // clock in data
        writePinHigh(MBI_DCLK_PIN);
        data = data << 1;
    }
    return recv;
}

/* Write val to each MBI's configuration registers.
 *
 * Note: order of array follows order from SDO to SDI pins.
 */
void mbi_write_configuration(uint16_t val) {
    mbi_send_instruction(MBI_ENABLE_WRITE_CONFIGURATION);
    int i = 0;
    for (; i < MBI_NUM_CASCADE - 1; i++) {
        mbi_shift_data(val << MBI_SHIFT_REG_OFFSET, MBI_SHIFT_REG_WIDTH);
    }
    if (i < MBI_NUM_CASCADE) {
        mbi_shift_data_instr(val << MBI_SHIFT_REG_OFFSET, MBI_SHIFT_REG_WIDTH, MBI_WRITE_CONFIGURATION);
    }
    return;
}

/* Find length of shift-register by clearing shift-register with 0s, writing
 * with 1s, and checking how many DCLK pulses until a 1 is detected.
 */
int mbi_get_shift_register_length(void) {
    int len = 0;
    writePinLow(MBI_LE_PIN);
    // clear shift register
    mbi_shift_data(0, MBI_NUM_CASCADE * MBI_SHIFT_REG_WIDTH);
    // write 1s until 1 appears on SDO
    int out = readPin(MBI_SDO_PIN);
    while (!out) {
        len++;
        mbi_shift_data(1U << 15, 1);
        out = readPin(MBI_SDO_PIN);
    }
    return len;
}
