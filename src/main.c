#include "SEGGER_RTT.h"
#include "pinout.h"
#include "app_error.h"
#include "app_pwm.h"
#include "include/spi.h"
#include "include/display.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include <string.h>

#if !defined(DEBUG)
#warning "Debug mode is not enabled!"
#endif

static const nrf_drv_timer_t LVGL_TIMER = NRF_DRV_TIMER_INSTANCE(0);

APP_PWM_INSTANCE(PWM1, 1);       // Create the instance "PWM1" using TIMER1.
static volatile bool ready_flag; // A flag indicating PWM status.

static const nrfx_spim_config_t SPI_CONFIG = {
    .sck_pin        = BOARD_SPI_SCK_PIN,
    .mosi_pin       = BOARD_SPI_MOSI_PIN,
    .miso_pin       = BOARD_SPI_MISO_PIN,
    .ss_pin         = BOARD_SPI_LCD_CS_PIN, // NRFX_SPIM_PIN_NOT_USED,
    .ss_active_high = false,
    .irq_priority   = NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc            = 0xFF,
    .frequency      = SPIM_FREQUENCY_FREQUENCY_M32,
    .mode           = NRF_SPIM_MODE_0,
    .bit_order      = NRF_SPIM_BIT_ORDER_MSB_FIRST,
};

static void
LVGL_TIMER_event_handler(nrf_timer_event_t event_type, void *p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            display_ticks();
            break;
        default:
            // Do nothing.
            break;
    }
}

static void
board_setup_timer(void)
{
    static uint32_t const time_ms = LVGL_LOOP_DELAY_MS; // Time(in miliseconds) between consecutive compare events.
    uint32_t              time_ticks;
    uint32_t              err_code = NRF_SUCCESS;

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code                         = nrf_drv_timer_init(&LVGL_TIMER, &timer_cfg, LVGL_TIMER_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&LVGL_TIMER, time_ms);

    nrf_drv_timer_extended_compare(&LVGL_TIMER, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&LVGL_TIMER);
}

static void
board_setup_gpios(void)
{
    nrf_gpio_cfg_output(BOARD_LCD_DC_PIN);
    nrf_gpio_cfg_output(BOARD_LCD_RST_PIN);
}

int
main(void)
{
    ret_code_t err_code;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SEGGER_RTT_Init();

    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(5000L, BOARD_BACKLIGHT_PIN);
    err_code                  = app_pwm_init(&PWM1, &pwm1_cfg, NULL);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM1);
    while (app_pwm_channel_duty_set(&PWM1, 0, 50) == NRF_ERROR_BUSY) {}

    /// Initialize SPI
    NRF_LOG_INFO("Initializing SPI...");
    NRF_LOG_FLUSH();
    spi_init(&SPI_CONFIG);

    /// Initialize timer
    NRF_LOG_INFO("Initializing timer...");
    NRF_LOG_FLUSH();
    board_setup_timer();

    /// Setup GPIOs
    NRF_LOG_INFO("Setting up GPIOs...");
    NRF_LOG_FLUSH();
    board_setup_gpios();

    /// Initialize Display
    int res = display_init();

    NRF_LOG_INFO("Starting main loop...");
    NRF_LOG_FLUSH();

    while (1)
    {
        display_flush();
        NRF_LOG_FLUSH();
        __WFI();
    }
}