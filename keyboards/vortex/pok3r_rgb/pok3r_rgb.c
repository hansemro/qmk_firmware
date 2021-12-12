/*
 * Copyright (c) 2018 Jonathan A. Kollasch
 *           (c) 2018 Charlie Waters
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pok3r_rgb.h"
#include "gd25q_flash.h"
#include "raw_hid.h"
#include "debug.h"
#include "version.h"

#include <stdint.h>
#include <string.h>

#define MIN(A, B) (A < B ? A : B)

#define RESET_FW_MAGIC  0x55aafaf0U
#define RESET_BL_MAGIC  0x55aafaf5U

#define EE_CONFIG_SIG       "CONF"
#define EE_CONFIG_SIG_LEN   4
#define EE_KEYMAP_SIG       "KEYM"
#define EE_KEYMAP_SIG_LEN   4

const uint8_t firmware_id[] __attribute__ ((section (".id.firmware"))) =
    "qmk_pok3r;" NAME_SLUG ";" QMK_VERSION ";" QMK_BUILDDATE;

void matrix_init_pins(void){
    const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
    for (uint8_t i = 0; i < MATRIX_COLS; ++i){
        palSetLineMode(col_pins[i],
                       PAL_MODE_ALTERNATE(AFIO_GPIO) | PAL_MODE_INPUT_PULLUP);
    }

    const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
    for (uint8_t i = 0; i < MATRIX_ROWS; ++i){
        palSetLineMode(row_pins[i],
                       PAL_MODE_ALTERNATE(AFIO_GPIO) | PAL_MODE_OUTPUT_PUSHPULL);
    }
}

void bootloader_jump(void) {
//    printf("Bootloader Jump\n");
    firmware_reset(RESET_BL_MAGIC);
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) { return false; }

    // If console is enabled, it will print the matrix position and status of each key pressed
#ifdef CONSOLE_ENABLE
    printf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif
    return true;
}

void virtser_recv(uint8_t c) {
#ifdef CONSOLE_ENABLE
    printf("VS: %c\n", c);
#endif
}
