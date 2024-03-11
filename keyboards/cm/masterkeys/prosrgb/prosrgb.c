// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "hal.h"
#include "wait.h"

#define SRAM_BOOT_MODE 0x40080304
#define RESET_AP_MAGIC 0x55aafaf0
#define RESET_IAP_MAGIC 0x55aafaf5

void bootloader_jump(void) {
    wait_us(10000);                // 10 ms
    chSysDisable();                // mask all interrupts
    usbDisconnectBus(&USB_DRIVER); // disconnect usb
    *((volatile uint32_t *)SRAM_BOOT_MODE) = RESET_IAP_MAGIC;
    wait_us(50000); // 50 ms
    NVIC_SystemReset();
}
