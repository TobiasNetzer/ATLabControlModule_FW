/*
 * led_animation.h
 *
 *  Created on: 04.06.2026
 *      Author: Tobias
 */

#ifndef INC_LED_ANIMATION_H_
#define INC_LED_ANIMATION_H_

#include <stdint.h>

typedef enum
{
    LED_GROUP_START = 0,
    LED_GROUP_STOP,
    LED_GROUP_STATUS,

    LED_GROUP_COUNT

} led_group_t;

typedef enum
{
    LED_MODE_OFF = 0,

    LED_MODE_RED,
    LED_MODE_GREEN,
    LED_MODE_BLUE,

    LED_MODE_BREATH_RED,
    LED_MODE_BREATH_GREEN,
    LED_MODE_BREATH_BLUE,

    LED_MODE_TEST_IDLE,
	LED_MODE_TEST_RUNNING,
	LED_MODE_TEST_PASSED,
	LED_MODE_TEST_FAILED,
	LED_MODE_TEST_CANCELLED

} led_mode_t;

void led_animation_init(void);

void led_animation_set_mode(
        led_group_t group,
        led_mode_t mode);

void led_animation_task(void);

#endif /* INC_LED_ANIMATION_H_ */
