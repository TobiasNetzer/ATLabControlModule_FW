/*
 * led_animation.c
 *
 *  Created on: 04.06.2026
 *      Author: Tobias
 */
#include "led_animation.h"
#include "led_driver.h"

#include "main.h"

typedef struct
{
    led_mode_t mode;

    uint8_t brightness;
    int8_t direction;

    uint32_t last_update;

} group_state_t;

static group_state_t groups[LED_GROUP_COUNT];

static void group_fill(
        led_group_t group,
        uint8_t r,
        uint8_t g,
        uint8_t b);

static void update_breath(
        group_state_t *state,
        uint8_t *brightness);

void led_animation_init(void)
{
    for(uint32_t i = 0; i < LED_GROUP_COUNT; i++)
    {
        groups[i].mode = LED_MODE_OFF;
        groups[i].brightness = 0;
        groups[i].direction = 1;
        groups[i].last_update = 0;
    }
}

void led_animation_set_mode(
        led_group_t group,
        led_mode_t mode)
{
    if(group >= LED_GROUP_COUNT)
        return;

    groups[group].mode = mode;
}

void led_animation_task(void)
{
    uint8_t brightness;

    for(uint32_t group = 0;
        group < LED_GROUP_COUNT;
        group++)
    {
        switch(groups[group].mode)
        {
            case LED_MODE_OFF:
                group_fill(group, 0, 0, 0);
                break;

            case LED_MODE_RED:
                group_fill(group, 255, 0, 0);
                break;

            case LED_MODE_GREEN:
                group_fill(group, 0, 255, 0);
                break;

            case LED_MODE_BLUE:
                group_fill(group, 0, 0, 255);
                break;

            case LED_MODE_BREATH_RED:
                update_breath(&groups[group], &brightness);
                group_fill(group, brightness, 0, 0);
                break;

            case LED_MODE_BREATH_GREEN:
                update_breath(&groups[group], &brightness);
                group_fill(group, 0, brightness, 0);
                break;

            case LED_MODE_BREATH_BLUE:
                update_breath(&groups[group], &brightness);
                group_fill(group, 0, 0, brightness);
                break;

            case LED_MODE_RAINBOW:
                /* TODO */
                break;

            default:
                break;
        }
    }
}

static void update_breath(
        group_state_t *state,
        uint8_t *brightness)
{
    uint32_t now = HAL_GetTick();

    if((now - state->last_update) >= 10)
    {
        state->last_update = now;

        state->brightness += state->direction;

        if(state->brightness >= 255)
        {
            state->brightness = 255;
            state->direction = -1;
        }

        if(state->brightness == 0)
        {
            state->direction = 1;
        }
    }

    *brightness = state->brightness;
}

static void group_fill(
        led_group_t group,
        uint8_t r,
        uint8_t g,
        uint8_t b)
{
    switch(group)
    {
        case LED_GROUP_START:
            led_set_pixel(0, r, g, b);
            break;

        case LED_GROUP_STOP:
            led_set_pixel(1, r, g, b);
            break;

        case LED_GROUP_STATUS:
            led_fill(2, 5, r, g, b);
            break;

        default:
            break;
    }
}
