/**
 ********************************************************************************
 * @file    spi.h
 * @author  Danijel Sipos
 * @date    10.09.2025
 * @brief   Implementation of spi
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025 INFI7 d.o.o. . All rights reserved.
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

#define NRF_SPI_MAX_TX_RX_SIZE 0xffff // 0xffff /**< Maximus size of spi buffer to write / write */

int8_t spi_init(nrfx_spim_config_t const *p_spi_config);
int8_t spi_write(const uint8_t *data, uint16_t data_len);
int8_t spi_read(uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */