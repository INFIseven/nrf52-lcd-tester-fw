/**
 ********************************************************************************
 * @file    display.h
 * @author  Danijel Sipos
 * @brief   ST7789 LCD display driver interface
 *
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to <http://unlicense.org/>
 ********************************************************************************
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#define LVGL_LOOP_DELAY_MS 5   /**< LVGL tick period in milliseconds */
#define DISPLAY_HOR_RES    170 /**< Display horizontal resolution in pixels */
#define DISPLAY_VER_RES    320 /**< Display vertical resolution in pixels */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the display and LVGL
 * @return 0 on success, negative on error
 *
 * Performs hardware reset sequence, initializes LVGL library, configures the
 * ST7789 driver, and displays the initial image.
 */
int8_t display_init(void);

/**
 * @brief Process LVGL tasks and flush display updates
 *
 * Should be called periodically from the main loop to handle LVGL rendering tasks.
 */
void   display_flush(void);

/**
 * @brief Update LVGL tick counter
 *
 * Should be called from timer interrupt at intervals of LVGL_LOOP_DELAY_MS.
 */
void   display_ticks(void);

#ifdef __cplusplus
}
#endif

#endif /* _DISPLAY_H_ */