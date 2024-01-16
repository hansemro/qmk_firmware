/*
    ChibiOS - Copyright (C) 2020 Codetector <codetector@codetector.cn>
    Copyright (C) 2022 Hansem Ro <hansemro@outlook.com>

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
 * Setup for Cooler Master Masterkeys Pro L White
 */

/*
 * Board identifier.
 */
#define BOARD_NAME "Masterkeys Pro L White"

#define HT32F52352

#define FLASH_SIZE (0x10000 - 0x4000) // 64kB - 16kB

/* MBI5042 */
#define MBI5042_NUM_CASCADE 1
#define MBI5042_LE_PIN A15
#define MBI5042_SDI_PIN C12
#define MBI5042_SDO_PIN B0
#define MBI5042_DCLK_PIN A14
#define MBI5042_GCLK_PIN C10
#define MBI5042_HAS_POWER_PIN
/* Enable 3.3V rail to MBI5042 when pin goes low */
#define MBI5042_PWRCTRL_PIN C11

#define LED_ROW_PINS \
    { C15, C14, B5, B4, B3, B2, D3, D2 }

#if !defined(_FROM_ASM_)
#    ifdef __cplusplus
extern "C" {
#    endif
void boardInit(void);
#    ifdef __cplusplus
}
#    endif
#endif /* _FROM_ASM_ */
