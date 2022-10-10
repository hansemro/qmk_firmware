/*
    ChibiOS - Copyright (C) 2020 Codetector <codetector@codetector.cn>

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
 * Setup for Cooler Master Masterkeys Pro S RGB
 */

/*
 * Board identifier.
 */
#define BOARD_NAME "MasterKeys Pro S RGB"

#define HT32F1654

#define FLASH_SIZE (0x10000 - 0x4000) // 64kB - 16kB

// MCU -> MBIA_R -> MBIA_G -> MBIA_G -> MCU
#define MBIA_COUNT 3 // one MBIA IC for each color channel
// MBIA pins
#define MBIA_GCLK_PIN       PAL_LINE(IOPORTC, 0)
#define MBIA_DCLK_PIN       PAL_LINE(IOPORTA, 14)
#define MBIA_SDI_PIN        PAL_LINE(IOPORTC, 2) // data sent to first MBIA from MCU
#define MBIA_SDO_PIN        PAL_LINE(IOPORTB, 0) // data sent to CPU from last MBIA
#define MBIA_LE_PIN         PAL_LINE(IOPORTA, 15)
// MBIA 3.3V power enable
#define MBIA_PWRCTRL_PIN    PAL_LINE(IOPORTC, 1)

#if !defined(_FROM_ASM_)
#    ifdef __cplusplus
extern "C" {
#    endif
void boardInit(void);
#    ifdef __cplusplus
}
#    endif
#endif /* _FROM_ASM_ */
