/*
 * led_driver.h
 *
 *  Created on: 03.06.2026
 *      Author: Tobias
 */

#ifndef INC_LED_DRIVER_H_
#define INC_LED_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#define LED_COUNT 7

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_pixel_t;

void led_driver_init(void);

void led_set_pixel(uint8_t index,
                   uint8_t r,
                   uint8_t g,
                   uint8_t b);

void led_fill(uint8_t first,
              uint8_t count,
              uint8_t r,
              uint8_t g,
              uint8_t b);

void led_show(void);

void led_driver_task(void);

bool led_driver_busy(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_LED_DRIVER_H_ */
