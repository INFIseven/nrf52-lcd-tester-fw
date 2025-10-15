/**
 ********************************************************************************
 * @file    display.c
 * @author  Danijel Sipos
 * @date    15.10.2025
 * @brief   Implementation of display
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 ********************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "include/display.h"
#include "include/spi.h"
#include "images/generated_images.h"
#include "pinout.h"
#include "lvgl.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"

#define BYTES_PER_PIXEL 2 // RGB565

#define ST7789_RESET_CLEAR(gpio_pin) nrf_gpio_pin_clear(gpio_pin) // TODO: dani brief what needed for function to link
#define ST7789_RESET_SET(gpio_pin)   nrf_gpio_pin_set(gpio_pin)
#define ST7789_DC_SET(gpio_pin)      nrf_gpio_pin_set(gpio_pin)
#define ST7789_DC_CLEAR(gpio_pin)    nrf_gpio_pin_clear(gpio_pin)
#define ST7789_WAIT(wait_ms)         nrf_delay_ms(wait_ms);

static void
display_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    if (cmd_size)
    {
        ST7789_DC_CLEAR(BOARD_LCD_DC_PIN);
        spi_write(cmd, cmd_size);
    }
    if (param_size)
    {
        ST7789_DC_SET(BOARD_LCD_DC_PIN);
        spi_write(param, param_size);
    }
}

static void
display_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    if (cmd_size)
    {
        ST7789_DC_CLEAR(BOARD_LCD_DC_PIN);
        spi_write(cmd, cmd_size);
    }
    if (param_size)
    {
        ST7789_DC_SET(BOARD_LCD_DC_PIN);
        spi_write(param, param_size);
    }
    lv_display_flush_ready(disp);
}

int8_t
display_init(void)
{
    ST7789_RESET_SET(BOARD_LCD_RST_PIN);
    ST7789_WAIT(150);
    ST7789_RESET_CLEAR(BOARD_LCD_RST_PIN);
    ST7789_WAIT(150);
    ST7789_RESET_SET(BOARD_LCD_RST_PIN);
    ST7789_WAIT(150);

    NRF_LOG_INFO("Initializing LVGL...");
    NRF_LOG_FLUSH();

    lv_init();

    lv_display_t *disp = lv_st7789_create(DISPLAY_HOR_RES, DISPLAY_VER_RES, 0, display_send_cmd, display_send_color);
    lv_st7789_set_gap(disp, 35, 0);
    lv_st7789_set_invert(disp, true);
    static uint8_t buf1[DISPLAY_HOR_RES * DISPLAY_VER_RES / 10 * BYTES_PER_PIXEL];
    /* Set display buffer for display `display1`. */
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565_SWAPPED);

    // Show image
    lv_obj_t *image = lv_image_create(lv_screen_active());
    lv_image_set_src(image, images[0]);

    return 0;
}

void
display_flush(void)
{
    lv_task_handler();
}

void
display_ticks(void)
{
    lv_tick_inc(LVGL_LOOP_DELAY_MS);
}