/*
 * Copyright (c) 2021 ChaoticEnigma
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

#include "eeprom_driver.h"

#include "hal.h"
#include "quantum.h"
#include "gd25q_flash.h"

#define FLASH_BYTE_COUNT    GD25Q40_BYTES
#define FLASH_PAGE_SIZE     GD25Q40_PAGE_BYTES

#if defined(DEBUG_EEPROM_OUTPUT)
    #define eeprom_printf(fmt, ...) xprintf(fmt, ##__VA_ARGS__);
#else
    #define eeprom_printf(fmt, ...)
#endif /* DEBUG_EEPROM_OUTPUT */

#define MIN(A, B) ((A) < (B) ? (A) : (B))

uint8_t page_buffer[FLASH_PAGE_SIZE];

static void spi_select(void) {
    palClearLine(EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN);
}

static void spi_deselect(void) {
    palSetLine(EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN);
}

static void spi_txrx(uint16_t n, const uint8_t *txbuf, uint8_t *rxbuf) {
    if (txbuf) {
        const uint8_t *txptr = txbuf;
        if (rxbuf) {
            uint8_t *rxptr = rxbuf;
            for (uint16_t i = 0; i < n; ++i) {
                *rxptr = spiPolledExchange(&SPI_DRIVER, *txptr);
                ++txptr;
                ++rxptr;
            }
        } else {
            for (uint16_t i = 0; i < n; ++i) {
                spiPolledExchange(&SPI_DRIVER, *txptr);
                ++txptr;
            }
        }
    } else {
        if (rxbuf) {
            uint8_t *rxptr = rxbuf;
            for (uint16_t i = 0; i < n; ++i) {
                *rxptr = spiPolledExchange(&SPI_DRIVER, 0);
                ++rxptr;
            }
        } else {
            for (uint16_t i = 0; i < n; ++i) {
                spiPolledExchange(&SPI_DRIVER, 0);
            }
        }
    }
}

// Send a 1-byte command
static void spi_cmd1(uint8_t cmd){
    spi_select();
    spi_txrx(1, &cmd, NULL);
    spi_deselect();
}

// Read SPI flash status register
static uint8_t spi_status(void) {
    uint8_t rx_data[2];

    const uint8_t tx_rdsrl[] = { GD25Q_RDSR_L, 0 };
    spi_select();
    // send command, read status
    spi_txrx(2, tx_rdsrl, rx_data); // read status register low
    spi_deselect();

    return rx_data[1];
}

// Poll SPI flash status until write done
void spi_wait_wip(void) {
    uint8_t rx_data;
    uint16_t count = 0;

    const uint8_t tx_rdsrl[] = { GD25Q_RDSR_L, 0 };
    spi_select();
    // send command
    spi_txrx(1, tx_rdsrl, NULL); // read status register low
    do {
        // read status byte
        spi_txrx(1, NULL, &rx_data);
        ++count;
    } while(rx_data & GD25Q_SR_WIP);
    spi_deselect();
}

// Read SPI flash identification
void spi_rdid(uint8_t *data) {
    uint8_t tx_data[4];
    tx_data[0] = GD25Q_RDID; // read id
    tx_data[1] = 0;
    tx_data[2] = 0;
    tx_data[3] = 0;

    spi_select();
    // send command, read id
    spi_txrx(4, tx_data, data);
    spi_deselect();
}

// Read flash data
static void read_flash(uint32_t addr, uint16_t n, uint8_t *data){
    eeprom_printf("Read data %x %d\n", addr, n);

    uint8_t tx_data[4];
    tx_data[0] = GD25Q_READ; // continuous read
    tx_data[1] = (addr >> 16) & 0xFF;
    tx_data[2] = (addr >> 8) & 0xFF;
    tx_data[3] = addr & 0xFF;

    spi_select();
    // send command and address
    spi_txrx(4, tx_data, NULL);
    // read back flash data
    spi_txrx(n, NULL, data);
    spi_deselect();
}

// Write flash page
static void write_flash_page(uint32_t addr, uint16_t n, const uint8_t *data) {
    const uint8_t page_offset = addr & 0xFF;
    if((page_offset + n) > GD25Q40_PAGE_BYTES){
        eeprom_printf("Invalid page write size\n");
        return;
    }

    eeprom_printf("Write page %x %d\n", addr, n);

    uint8_t tx_data[4];
    tx_data[0] = GD25Q_PP; // page program
    tx_data[1] = (addr >> 16) & 0xFF;
    tx_data[2] = (addr >> 8) & 0xFF;
    tx_data[3] = addr & 0xFF;

    if (spi_status() & GD25Q_SR_WIP) {
        eeprom_printf("Device busy\n");
        return;
    }

    spi_cmd1(GD25Q_WREN);
    wait_us(1);

    spi_select();
    // send command and address
    spi_txrx(4, tx_data, NULL);
    // send flash data
    spi_txrx(n, data, NULL);
    spi_deselect();
}

static void erase_flash_page(uint32_t addr){
    uint8_t tx_data[4];
    tx_data[0] = GD25Q_SE; // sector erase
    tx_data[1] = (addr >> 16) & 0xFF;
    tx_data[2] = (addr >> 8) & 0xFF;
    tx_data[3] = addr & 0xFF;

    if (spi_status() & GD25Q_SR_WIP) {
        printf("Device busy\n");
        return;
    }

    spi_cmd1(GD25Q_WREN);
    wait_us(1);

    spi_select();
    // send command and address
    spi_txrx(4, tx_data, NULL);
    spi_deselect();
}

//
// Public functions
//

void eeprom_driver_init(void) {
    /* Any initialisation code */
}

void eeprom_driver_erase(void) {
    /* Wipe out the EEPROM, setting values to zero */
    spi_cmd1(GD25Q_CE);
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    /*
        Read a block of data:
            buf: target buffer
            addr: 0-based offset within the EEPROM
            len: length to read
     */
    uint32_t raddr = (uint32_t)addr;
    read_flash(raddr, len, buf);
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    /*
        Write a block of data:
            buf: target buffer
            addr: 0-based offset within the EEPROM
            len: length to write
     */
    uint32_t waddr = (uint32_t)addr;
    size_t rem_len = len;

    // Unaligned first page
    const uint8_t page_offset = waddr & 0xFF;
    if (page_offset != 0) {
        const uint32_t first_page_addr = waddr & 0xFFFFFF00;
        const uint8_t first_page_len = MIN(rem_len,FLASH_PAGE_SIZE - page_offset);
        // Read unchanged page data
        read_flash(first_page_addr, page_offset, page_buffer);
        // Set new page data
        memcpy(page_buffer + page_offset, buf, first_page_len);
        // Erase and write page
        spi_wait_wip();
        erase_flash_page(first_page_addr);
        spi_wait_wip();
        write_flash_page(first_page_addr, FLASH_PAGE_SIZE, page_buffer);

        waddr += first_page_len;
        buf += first_page_len;
        rem_len -= first_page_len;
    }

    // Aligned full pages
    while(rem_len >= FLASH_PAGE_SIZE) {
        spi_wait_wip();
        erase_flash_page(waddr);
        spi_wait_wip();
        write_flash_page(waddr, FLASH_PAGE_SIZE, buf);

        waddr += FLASH_PAGE_SIZE;
        buf += FLASH_PAGE_SIZE;
        rem_len -= FLASH_PAGE_SIZE;
    }

    // Last partial page
    if (rem_len != 0) {
        // Read unchanged page data
        read_flash(waddr + rem_len, FLASH_PAGE_SIZE - rem_len, page_buffer + rem_len);
        // Set new page data
        memcpy(page_buffer, buf, rem_len);
        // Erase and write page
        spi_wait_wip();
        erase_flash_page(waddr);
        spi_wait_wip();
        write_flash_page(waddr, FLASH_PAGE_SIZE, page_buffer);
    }
}
