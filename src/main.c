#include "SEGGER_RTT.h"
#include "pinout.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "app_pwm.h"
#include "include/spi.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_spi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include <string.h>
#include "lvgl.h"

#if !defined(DEBUG)
#warning "Debug mode is not enabled!"
#endif

#define LVGL_LOOP_DELAY_MS 5
#define MY_DISP_HOR_RES    170
#define MY_DISP_VER_RES    320
#define BYTES_PER_PIXEL    2 // RGB565

#define ST7789_RESET_CLEAR(gpio_pin)   nrf_gpio_pin_clear(gpio_pin) // TODO: dani brief what needed for function to link
#define ST7789_RESET_SET(gpio_pin)     nrf_gpio_pin_set(gpio_pin)
#define ST7789_CHIP_SELECT(gpio_pin)   nrf_gpio_pin_clear(gpio_pin)
#define ST7789_CHIP_UNSELECT(gpio_pin) nrf_gpio_pin_set(gpio_pin)
#define ST7789_DC_SET(gpio_pin)        nrf_gpio_pin_set(gpio_pin)
#define ST7789_DC_CLEAR(gpio_pin)      nrf_gpio_pin_clear(gpio_pin)

const nrf_drv_timer_t LVGL_TIMER = NRF_DRV_TIMER_INSTANCE(0);

#if 1
APP_PWM_INSTANCE(PWM1, 1);       // Create the instance "PWM1" using TIMER1.
static volatile bool ready_flag; // A flag indicating PWM status.
#endif

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
my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    NRF_LOG_INFO("cmd");
}

static void
my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    NRF_LOG_INFO("color");
}

static void
LVGL_TIMER_event_handler(nrf_timer_event_t event_type, void *p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            lv_tick_inc(LVGL_LOOP_DELAY_MS);
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

int
main(void)
{
    ret_code_t err_code;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SEGGER_RTT_Init();

#if 1
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(5000L, BOARD_BACKLIGHT_PIN);
    err_code                  = app_pwm_init(&PWM1, &pwm1_cfg, NULL);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM1);
    while (app_pwm_channel_duty_set(&PWM1, 0, 50) == NRF_ERROR_BUSY) {}
#endif

#if 1
    /// Initialize SPI
    NRF_LOG_INFO("Initializing SPI...");
    NRF_LOG_FLUSH();
    spi_init(&SPI_CONFIG);
#endif

    /// Initialize timer
    NRF_LOG_INFO("Initializing timer...");
    board_setup_timer();

    /// Initialize LVGL
    NRF_LOG_INFO("Initializing LVGL...");
    NRF_LOG_FLUSH();
    lv_init();
    lv_display_t  *disp = lv_st7789_create(MY_DISP_HOR_RES, MY_DISP_VER_RES, 0, my_lcd_send_cmd, my_lcd_send_color);
    static uint8_t buf1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTES_PER_PIXEL];
    /* Set display buffer for display `display1`. */
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // /*Change the active screen's background color*/
    // lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    // /*Create a white label, set its text and align it to the center*/
    // lv_obj_t *label = lv_label_create(lv_screen_active());
    // lv_label_set_text(label, "Hello world");
    // lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    NRF_LOG_INFO("Starting main loop...");
    NRF_LOG_FLUSH();

    while (1)
    {
        lv_task_handler();
        NRF_LOG_FLUSH();
        __WFI();
    }
}