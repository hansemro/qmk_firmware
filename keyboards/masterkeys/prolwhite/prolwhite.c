/* Copyright 2022 Mateusz Radomski
 * Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
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

#include "hal.h"
#include "quantum.h"
#include "mbi5042.h"

#define SRAM_BOOT_MODE 0x20003ffc
#define RESET_AP_MAGIC 0x55aafaf0
#define RESET_IAP_MAGIC 0x55aafaf5
#define WATCH_DOG_CR_ADDR 0x40068000
#define WATCH_DOG_RELOAD_VALUE 0x5fa00001

void reload_watchdog(void) {
    *(uint32_t *)(WATCH_DOG_CR_ADDR) = WATCH_DOG_RELOAD_VALUE;
}

void bootloader_jump(void) {
    wait_us(10000);                // 10 ms
    chSysDisable();                // mask all interrupts
    usbDisconnectBus(&USB_DRIVER); // disconnect usb
    *((volatile uint32_t *)SRAM_BOOT_MODE) = RESET_IAP_MAGIC;
    wait_us(50000); // 50 ms
    NVIC_SystemReset();
}

void matrix_scan_kb() {
    reload_watchdog();
    matrix_scan_user();
}

#ifdef LED_MATRIX_ENABLE
bool led_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    mbi5042_set_mask(62, host_keyboard_led_state().caps_lock ? 0xff : 0);
    mbi5042_set_mask(14, host_keyboard_led_state().scroll_lock ? 0xff : 0);
    mbi5042_set_mask(37, host_keyboard_led_state().num_lock ? 0xff : 0);
    return false;
}
#endif
