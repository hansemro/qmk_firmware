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

#if !defined(_FROM_ASM_)
#    ifdef __cplusplus
extern "C" {
#    endif
void boardInit(void);
#    ifdef __cplusplus
}
#    endif
#endif /* _FROM_ASM_ */
