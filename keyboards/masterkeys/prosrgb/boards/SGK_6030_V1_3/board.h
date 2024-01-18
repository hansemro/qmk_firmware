// Copyright (C) 2020 Codetector <codetector@codetector.cn>
// Copyright 2024 Hansem Ro <hansemro@outlook.com>
// SPDX-License-Identifier: Apache-2.0

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

#if !defined(_FROM_ASM_)
#    ifdef __cplusplus
extern "C" {
#    endif
void boardInit(void);
#    ifdef __cplusplus
}
#    endif
#endif /* _FROM_ASM_ */
