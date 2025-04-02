// Copyright (c) 2021 Michael Stoops. All rights reserved.
// Portions copyright (c) 2021 Raspberry Pi (Trading) Ltd.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
// following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
//    disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//    following disclaimer in the documentation and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
//    products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Example of an SPI bus master using the PL022 SPI interface

#include <stdio.h>
#include "pico/stdlib.h"
#include "hw/spi/hw_spi.h"

#define BUF_LEN 0x100

void printbuf(uint8_t buf[], size_t len)
{
    size_t i;
    for (i = 0; i < len; ++i)
    {
        if (i % 16 == 15)
            printf("%02x\n", buf[i]);
        else
            printf("%02x ", buf[i]);
    }

    // append trailing newline if there isn't one
    if (i % 16)
    {
        putchar('\n');
    }
}

struct_ConfigMasterSPI cfg = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 1000 * 1000};

int main()
{
    spi_inst_t *spi = spi1;
    HW_SPI_Master master = HW_SPI_Master(cfg);
    // Enable UART so we can print
    stdio_init_all();
     printf("SPI master example\n");

    uint8_t out_buf[BUF_LEN], in_buf[BUF_LEN];

    // Initialize output buffer
    for (size_t i = 0; i < BUF_LEN; ++i)
    {
        out_buf[i] = i;
    }

    printf("SPI master says: The following buffer will be written to MOSI endlessly:\n");
    printbuf(out_buf, BUF_LEN);

    for (size_t i = 0;; ++i)
    {
        // Write the output buffer to MOSI, and at the same time read from MISO.
        master.burst_write_read_8(out_buf,in_buf,BUF_LEN);
 
        // Write to stdio whatever came in on the MISO line.
        printf("SPI master says: read page %d from the MISO line:\n", i);
        printbuf(in_buf, BUF_LEN);

        // Sleep for ten seconds so you get a chance to read the output.
        sleep_ms(10 * 1000);
    }
    // #endif
}