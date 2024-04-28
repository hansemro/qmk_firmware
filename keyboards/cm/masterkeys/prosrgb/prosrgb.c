// Copyright (c) 2024 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "hal.h"
#include "wait.h"

extern uint32_t __ram0_end__;

#define SYMVAL(sym) (uint32_t)(((uint8_t *)&(sym)) - ((uint8_t *)0))
#define MAGIC_ADDR (unsigned long *)(SYMVAL(__ram0_end__) - 4)
#define BOOTLOADER_MAGIC 0xDEADBEEF

__attribute__((weak)) void bootloader_marker_enable(void) {
    uint32_t *marker = (uint32_t *)MAGIC_ADDR;
    *marker          = BOOTLOADER_MAGIC; // set magic flag => reset handler will jump into boot loader
}

__attribute__((weak)) bool bootloader_marker_active(void) {
    const uint32_t *marker = (const uint32_t *)MAGIC_ADDR;
    return (*marker == BOOTLOADER_MAGIC);
}

__attribute__((weak)) void bootloader_marker_disable(void) {
    uint32_t *marker = (uint32_t *)MAGIC_ADDR;
    *marker          = 0;
}

__attribute__((weak)) void bootloader_jump(void) {
    bootloader_marker_enable();
    NVIC_SystemReset();
}

__attribute__((weak)) void mcu_reset(void) {
    NVIC_SystemReset();
}

void enter_bootloader_mode_if_requested(void) {
    if (bootloader_marker_active()) {
        bootloader_marker_disable();

        struct system_memory_vector_t {
            uint32_t stack_top;
            void (*entrypoint)(void);
        };
        const struct system_memory_vector_t *bootloader = (const struct system_memory_vector_t *)(0x1f000000);

        __disable_irq();

        SysTick->CTRL = 0;
        SysTick->VAL  = 0;
        SysTick->LOAD = 0;

        // Clear interrupt enable and interrupt pending registers
        for (int i = 0; i < ARRAY_SIZE(NVIC->ICER); i++) {
            NVIC->ICER[i] = 0xFFFFFFFF;
            NVIC->ICPR[i] = 0xFFFFFFFF;
        }
        __set_CONTROL(0);
        __set_MSP(bootloader->stack_top);

        FMC->SBVT[0] =  0x1f000000;
        FMC->SBVT[1] =  0x1f000004;
        FMC->SBVT[2] =  0x1f000008;
        FMC->SBVT[3] =  0x1f00000c;

        __enable_irq();

        // Jump to bootloader
        bootloader->entrypoint();
        while (true) {
        }
    }
}
