/**
 ********************************************************************************
 * @file    spi.h
 * @author  Danijel Sipos
 * @brief   SPI driver interface for NRF52
 *
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to <http://unlicense.org/>
 ********************************************************************************
 */

#ifndef _SPI_H_
#define _SPI_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_drv_spi.h"
#include <nrfx.h>
#include <nrfx_spim.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NRF_SPI_MAX_TX_RX_SIZE 0xffff /**< Maximum size of SPI buffer to write/read */

/**
 * @brief Initialize the SPI peripheral
 * @param p_spi_config Pointer to SPI configuration structure
 * @return 0 on success, -1 on error
 */
int8_t spi_init(nrfx_spim_config_t const *p_spi_config);

/**
 * @brief Write data via SPI
 * @param data Pointer to data buffer to transmit
 * @param data_len Length of data in bytes
 * @return 0 on success, -1 on error
 */
int8_t spi_write(const uint8_t *data, uint16_t data_len);

/**
 * @brief Read data via SPI
 * @param data Pointer to buffer for received data
 * @param data_len Number of bytes to read
 * @return 0 on success, -1 on error
 */
int8_t spi_read(uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */