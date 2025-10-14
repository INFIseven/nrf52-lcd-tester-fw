#include "SEGGER_RTT.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "include/spi.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include <string.h>

static const nrfx_spim_config_t SPI_CONFIG = {
    .sck_pin        = 1,
    .mosi_pin       = 2,
    .miso_pin       = 3,
    .ss_pin         = NRFX_SPIM_PIN_NOT_USED,
    .ss_active_high = false,
    .irq_priority   = NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc            = 0xFF,
    .frequency      = SPIM_FREQUENCY_FREQUENCY_M32,
    .mode           = NRF_SPIM_MODE_0,
    .bit_order      = NRF_SPIM_BIT_ORDER_MSB_FIRST,
};

int
main(void)
{

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SEGGER_RTT_Init();

    spi_init(&SPI_CONFIG);

    NRF_LOG_INFO("SPI example started.");

    while (1)
    {
        NRF_LOG_FLUSH();
        NRF_LOG_INFO("OK.");
        nrf_delay_ms(1200);
    }
}