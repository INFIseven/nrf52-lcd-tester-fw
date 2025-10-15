/**
 ********************************************************************************
 * @file    spi.c
 * @author  Danijel Sipos
 * @brief   SPI driver implementation for NRF52
 *
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to <http://unlicense.org/>
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

/**
 * @brief SPI event handler callback
 * @param p_event Pointer to SPI event structure (unused)
 * @param p_context User context pointer (unused)
 *
 * Sets the transfer done flag when SPI transaction completes.
 */
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

int8_t
spi_write(const uint8_t *data, uint16_t data_len)
{
    ret_code_t                  err_state      = 0;
    nrfx_spim_xfer_desc_t const spim_xfer_desc = {
        .p_tx_buffer = data,
        .tx_length   = data_len,
        .p_rx_buffer = NULL,
        .rx_length   = 0,
    };

    spi_xfer_done = false;
    err_state     = nrfx_spim_xfer(&SPI_INSTANCE, &spim_xfer_desc, 0);
    if (err_state != 0)
    {
        NRF_LOG_ERROR("SPI TX transfer failed: %d", err_state);
        return -1;
    }
    while (!spi_xfer_done)
    {
        __WFE();
    }
    return 0;
}

int8_t
spi_read(uint8_t *data, uint16_t data_len)
{
    ret_code_t err_state = 0;

    nrfx_spim_xfer_desc_t const spim_xfer_desc = {
        .p_tx_buffer = NULL,
        .tx_length   = 0,
        .p_rx_buffer = data,
        .rx_length   = data_len,
    };

    spi_xfer_done = false;
    err_state     = nrfx_spim_xfer(&SPI_INSTANCE, &spim_xfer_desc, 0);
    if (err_state != 0)
    {
        NRF_LOG_ERROR("SPI RX transfer failed: %d", err_state);
        return -1;
    }
    while (!spi_xfer_done) // TODO add timeout
    {
        __WFE();
    }

    return 0;
}