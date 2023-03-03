/*
    ChibiOS - Copyright (C) 2020 Codetector <codetector@codetector.cn>
              Copyright (c) 2022 Hansem Ro <hansemro@outlook.com>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once
/*
 * Setup for Cooler Master Masterkeys Pro M RGB
 */

/*
 * Board identifier.
 */
#define BOARD_NAME "MasterKeys Pro M RGB"

#define HT32F1654

#define FLASH_SIZE (0x10000 - 0x4000) // 64kB - 16kB

/* MCU -> MBIA_R -> MBIA_G -> MBIA_B -> MCU */
#define MBIA043_NUM_CASCADE 3 /* one MBIA IC for each color channel */
/* MBIA pins */
#define MBIA043_GCLK_PIN C0
#define MBIA043_DCLK_PIN A14
#define MBIA043_SDI_PIN C2 /* data sent to first MBIA from MCU */
#define MBIA043_SDO_PIN B0 /* data sent to MCU from last MBIA */
#define MBIA043_LE_PIN A15
/* MBIA 3.3V power enable */
#define MBIA043_HAS_POWER_PIN
/* 3.3V supply to MBIA ICs is enabled when C1 is low */
#define MBIA043_PWRCTRL_PIN C1

/* LED ROW GPIO Pins */
#define LED_ROW_PINS \
    { C8, C7, B5, B4, B3, B2, C6, C5 }

#define MBIA043_CONFIGURATION \
    { 0xc, 0xc, 0xc }

#if !defined(_FROM_ASM_)
#    ifdef __cplusplus
extern "C" {
#    endif
void boardInit(void);
#    ifdef __cplusplus
}
#    endif
#endif /* _FROM_ASM_ */
