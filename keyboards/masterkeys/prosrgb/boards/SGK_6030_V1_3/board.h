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
 * Setup for Cooler Master Masterkeys Pro S RGB
 */

/*
 * Board identifier.
 */
#define BOARD_NAME "MasterKeys Pro S RGB"

#define HT32F1654

#define FLASH_SIZE (0x10000 - 0x4000) // 64kB - 16kB

/* MCU -> MBIA_R -> MBIA_G -> MBIA_G -> MCU */
#define MBIA043_NUM_CASCADE 3 // one MBIA IC for each color channel
/* MBIA pins */
#define MBIA043_GCLK_PIN       PAL_LINE(IOPORTC, 0)
#define MBIA043_DCLK_PIN       PAL_LINE(IOPORTA, 14)
#define MBIA043_SDI_PIN        PAL_LINE(IOPORTC, 2) /* data sent to first MBIA from MCU */
#define MBIA043_SDO_PIN        PAL_LINE(IOPORTB, 0) /* data sent to CPU from last MBIA */
#define MBIA043_LE_PIN         PAL_LINE(IOPORTA, 15)
/* MBIA 3.3V power enable */
#define MBIA043_HAS_POWER_PIN
#define MBIA043_PWRCTRL_PIN    PAL_LINE(IOPORTC, 1)

/* LED (Anode) ROW GPIO Pins*/
#define Q1_Ctrl PAL_LINE(IOPORTC, 8)
#define Q2_Ctrl PAL_LINE(IOPORTC, 7)
#define Q3_Ctrl PAL_LINE(IOPORTB, 5)
#define Q4_Ctrl PAL_LINE(IOPORTB, 4)
#define Q5_Ctrl PAL_LINE(IOPORTB, 3)
#define Q6_Ctrl PAL_LINE(IOPORTB, 2)
#define Q7_Ctrl PAL_LINE(IOPORTC, 6)
#define Q8_Ctrl PAL_LINE(IOPORTC, 5)

#if !defined(_FROM_ASM_)
#    ifdef __cplusplus
extern "C" {
#    endif
void boardInit(void);
#    ifdef __cplusplus
}
#    endif
#endif /* _FROM_ASM_ */
