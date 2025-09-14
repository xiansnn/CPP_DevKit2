/**
 * @file rtos_hw_spi.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "hardware/spi.h"
#include "hw/spi/hw_spi.h"
#include "hw/dma/hw_dma.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "pico/stdlib.h"
#include <cstdint>

/// @brief strusture of data block sent to queue in order to start SPI emission
struct struct_TX_DataQueueSPI
{
  /// @brief  the pointer to the data to send
  void *data;
  /// @brief  the excpected nuber of transfer
  size_t length;
};

/**
 * @brief This is a C++ wrapper for the original pico SDK spi master API, adapted to FreeRTOS environment
 * \ingroup hw
 */
class rtos_HW_SPI_Master : public HW_SPI_Master
{
private:
public:
  /// @brief the DMA channel used for SPI RX
  HW_DMA *dma_rx;

  /// @brief the DMA channel used for SPI TX
  HW_DMA *dma_tx;


  /// @brief Construct a new hw spi master object
  /// @param master_config the data structure that contains SPI configuration
  /// @param dma_tx_irq_number the DMA IRQ number attached to TX DMA
  /// @param dma_tx_irq_handler the DMA IRQ handler attached to TX DMA
  /// @param dma_rx_irq_number the DMA IRQ number attached to RX DMA
  /// @param dma_rx_irq_handler the DMA IRQ handler attached to RX DMA
  rtos_HW_SPI_Master(struct_ConfigMasterSPI master_config,
                     irq_num_t dma_tx_irq_number, irq_handler_t dma_tx_irq_handler,
                     irq_num_t dma_rx_irq_number, irq_handler_t dma_rx_irq_handler);
  ~rtos_HW_SPI_Master();

  /**
   * @brief send a serie of 8 bit data on the SPI MOSI port
   * DMA not implemented
   * @param src the source of the data
   * @param len the lenght of the serie
   * @return int the number of byte sent
   */
  int burst_write_8(uint8_t *src, size_t len);

  /**
   * @brief send a serie of 16 bit data on the SPI MOSI port.
   * Use DMA.
   *
   * @param src the source of the data
   * @param len the lenght of the serie
   * @return error code
   */
  int burst_write_16(uint16_t *src, size_t len);

  /// @brief read a serie of 16 bit data on the SPI MISO port.
  /// Use DMA.
  /// @param dst the destination of the data
  /// @param len the length of the serie
  /// @return error code
  int burst_read_16(uint16_t *dst, size_t len);

  /**
   * @brief read / write a serie of 8 bit data on the SPI MISO/MOSI ports
   * DMA not implemented
   * @param src the source of the data
   * @param dest the destination of the data
   * @param len the length of the serie
   * @return error code
   */
  int burst_write_read_8(uint8_t *src, uint8_t *dest, size_t len);

  /**
   * @brief read a serie of byte on the SPI MISO port
   * DMA not implemented
   * @param repeated_tx_data the byte sent on MOSI port during the read transfer
   * @param dest the destination that receives the data
   * @param len the length of the serie
   * @return error code
   */
  int burst_read_8(uint8_t repeated_tx_data, uint8_t *dest, size_t len);

  /// @brief ISR handler for SPI TX DMA
  void spi_tx_dma_isr();

  /// @brief ISR handler for SPI RX DMA
  void spi_rx_dma_isr();
};