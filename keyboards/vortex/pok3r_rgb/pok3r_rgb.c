/* Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>
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

#include <stdint.h>
#include "hal.h"
#include "wait.h"
#include "quantum.h"

#define FMC_SBVT1 0x40080304
#define RESET_AP_MAGIC 0x55aafaf0
#define RESET_IAP_MAGIC 0x55aafaf5

void bootloader_jump(void) {
    wait_us(10000);                // 10 ms
    chSysDisable();                // mask all interrupts
    usbDisconnectBus(&USB_DRIVER); // disconnect usb
    *((uint32_t *)FMC_SBVT1) = RESET_IAP_MAGIC;
    wait_us(50000); // 50 ms
    NVIC_SystemReset();
}
