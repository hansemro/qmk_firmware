/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio
                        (C) 2018 Charlie Waters

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

#include "ch.h"
#include "hal.h"

#include "quantum.h"

void __early_init(void) {
//    ht32_clock_init();
}

static const SPIConfig spi1_config = {
    .end_cb = NULL,
    .cr0 = SPI_CR0_SELOEN,
    .cr1 = 8 | SPI_CR1_FORMAT_MODE0 | SPI_CR1_MODE,
    .cpr = 1,
    .fcr = 0,
};

/**
 * @brief   Board-specific initialization code.
 * @todo    Add your board-specific code, if any.
 */
void boardInit(void) {
    // SPI Flash (EEPROM) pins
    palSetLineMode(B7, PAL_MODE_ALTERNATE(AFIO_SPI));
    palSetLineMode(B8, PAL_MODE_ALTERNATE(AFIO_SPI));
    palSetLineMode(B9, PAL_MODE_ALTERNATE(AFIO_SPI));
    palSetLineMode(B10, PAL_MODE_ALTERNATE(AFIO_GPIO) | PAL_MODE_OUTPUT_PUSHPULL);

    spiStart(&SPI_DRIVER, &spi1_config);
    palSetLine(EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN);
//    spi_init();
}

void _unhandled_exception(void){
    while(true);
}


const uint8_t bootloader_magic[] __attribute__ ((section (".magic.bootloader"))) = {
    0x00, 0x00, 0x04, 0x03, // builtin bootloader CRC will fail if this word is not correct
    0x01, 0x00, 0x00, 0x34,
    0x04, 0x00, 0x02, 0x00, // probably not necessary
};
