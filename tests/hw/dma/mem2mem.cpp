#include <stdio.h>
#include "pico/stdlib.h"
#include "hw/dma/hw_dma.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);

#define TEST_SIZE 1024

void m2m_dma_handler();

HW_DMA dma = HW_DMA(DMA_IRQ_0, m2m_dma_handler);

// static struct_ConfigDMA cfg_dma{
//     .transfer_size = DMA_SIZE_16,
//     .number_of_transfer = TEST_SIZE,
//     .dma_irq_handler = m2m_dma_handler,
//     .irq_number = DMA_IRQ_0,
// };

static uint16_t read_buf[TEST_SIZE];
static uint16_t write_buf[TEST_SIZE];

void m2m_dma_handler()
{
    if (dma_hw->ints0 & (1u << dma.channel))
    {
        p2.hi();
        dma_hw->ints0 = (1u << dma.channel); // Clear IRQ
        p2.lo();
    }
}

int main()
{
    stdio_init_all();

    for (uint i = 0; i < TEST_SIZE; ++i)
        read_buf[i] = i;

    while (true)
    {
        p0.hi();
        dma.xfer_mem2mem(TEST_SIZE, DMA_SIZE_16,write_buf,read_buf,false);
        p1.hi();
        dma.start_dma();
        dma_channel_wait_for_finish_blocking(dma.channel);
        p1.lo();

        p3.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            if (write_buf[i] != read_buf[i])
            {
                panic("Mismatch at %d/%d: expected %02x, got %02x",
                      i, TEST_SIZE, read_buf[i], write_buf[i]);
            }
        }
        printf("All good\n");
        p3.lo();

        // dma.cleanup_and_free_dma_channel();
        p0.lo();

        sleep_ms(500);
    }
}