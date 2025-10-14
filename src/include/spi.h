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

int8_t spi_init(nrfx_spim_config_t const *p_spi_config);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */