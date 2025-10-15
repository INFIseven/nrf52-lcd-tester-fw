/**
 ********************************************************************************
 * @file    display.h
 * @author  Danijel Sipos
 * @date    15.10.2025
 * @brief   Implementation of display
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025 COMPANY_NAME. All rights reserved.
 ********************************************************************************
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#define LVGL_LOOP_DELAY_MS 5
#define DISPLAY_HOR_RES    170
#define DISPLAY_VER_RES    320

#ifdef __cplusplus
extern "C" {
#endif

int8_t display_init(void);
void   display_flush(void);
void   display_ticks(void);

#ifdef __cplusplus
}
#endif

#endif /* _DISPLAY_H_ */