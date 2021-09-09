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

#include "quantum.h"
#include "raw_hid.h"
#include "debug.h"
#include "version.h"

#include "pok3r_rgb.h"
#include "gd25q_flash.h"

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

void bootloader_jump(void) {
//    printf("Bootloader Jump\n");
    firmware_reset(RESET_BL_MAGIC);
}

void check_boot_keys(void) {
#if HAS_WIPE_SWITCH
    // check DIP switch and erase version page if set
//    if (matrix_scan_key(WIPE_SWITCH_ROW, WIPE_SWITCH_COL)) {
//        flash_erase_page(VERSION_ADDR);
//    }
#endif
}
