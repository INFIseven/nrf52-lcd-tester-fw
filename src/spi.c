/**
 ********************************************************************************
 * @file    spi.c
 * @author  Danijel Sipos
 * @date    10.09.2025
 * @brief   Implementation of spi
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025 INFI7 d.o.o. . All rights reserved.
 ********************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "include/spi.h"

#define NRF_LOG_MODULE_NAME spi
#define NRF_LOG_LEVEL       0
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static const nrfx_spim_t SPI_INSTANCE  = NRFX_SPIM_INSTANCE(0);
static volatile bool     spi_xfer_done = false;

static void
hal_driver_spi_event_handler(nrfx_spim_evt_t const *p_event, void *p_context)
{
    (void)p_event;
    (void)p_context;

    spi_xfer_done = true;
}

int8_t
spi_init(nrfx_spim_config_t const *p_spi_config)
{
    ret_code_t err_code = nrfx_spim_init(&SPI_INSTANCE, p_spi_config, hal_driver_spi_event_handler, NULL);

    return err_code == NRFX_SUCCESS ? 0 : -1;
}