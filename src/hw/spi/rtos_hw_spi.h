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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "pico/stdlib.h"
#include <cstdint>


/**
 * @brief This is a C++ wrapper for the original pico SDK spi master API, adapted to FreeRTOS environment
 * \ingroup hw     
 */
class rtos_HW_SPI_Master : public HW_SPI_Master
{
private:


public:
     /**
      * @brief Construct a new hw spi master object
      * 
      * @param master_config 
      */
    rtos_HW_SPI_Master(struct_ConfigMasterSPI master_config);
    ~rtos_HW_SPI_Master();

    /**
     * @brief send a serie of 8 bit data on the SPI MOSI port
     *
     * @param src the source of the data
     * @param len the lenght of the serie
     * @return int the number of byte sent
     */
    int burst_write_8(uint8_t *src, size_t len);
    /**
     * @brief send a serie of 16 bit data on the SPI MOSI port
     *
     * @param src the source of the data
     * @param len the lenght of the serie
     * @return int the number of byte sent
     */
    int burst_write_16(uint16_t *src, size_t len);

    /**
     * @brief read / write a serie of 8 bit data on the SPI MISO/MOSI ports
     *
     * @param src the source of the data
     * @param dest the destination of the data
     * @param len the length of the serie
     * @return int
     */
    int burst_write_read_8(uint8_t *src, uint8_t *dest, size_t len);

    

    
    /**
     * @brief read a serie of byte on the SPI MISO port
     *
     * @param repeated_tx_data the byte sent on MOSI port during the read transfer
     * @param dest the destination that receives the data
     * @param len the length of the serie
     * @return int
     */
    int burst_read_8(uint8_t repeated_tx_data, uint8_t *dest, size_t len);

};