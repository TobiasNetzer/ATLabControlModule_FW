/*
 * led_driver.c
 *
 *  Created on: 03.06.2026
 *      Author: Tobias
 */
#include "led_driver.h"
#include "main.h"

#include <string.h>

extern TIM_HandleTypeDef htim1;

#define TIMER_PERIOD   79

#define SK6805_ONE     60
#define SK6805_ZERO    30

#define RESET_SLOTS    50

static led_pixel_t leds[LED_COUNT];

static bool dirty = false;
static volatile bool dma_busy = false;

static uint16_t pwm_buffer[(LED_COUNT * 24) + RESET_SLOTS];

static void build_pwm_buffer(void);

void led_driver_init(void)
{
    memset(leds, 0, sizeof(leds));

    dirty = true;
    dma_busy = false;
}

bool led_driver_busy(void)
{
    return dma_busy;
}

void led_set_pixel(uint8_t index,
                   uint8_t r,
                   uint8_t g,
                   uint8_t b)
{
    if(index >= LED_COUNT)
        return;

    if(leds[index].r == r &&
       leds[index].g == g &&
       leds[index].b == b)
    {
        return;
    }

    leds[index].r = r;
    leds[index].g = g;
    leds[index].b = b;

    dirty = true;
}

void led_fill(uint8_t first,
              uint8_t count,
              uint8_t r,
              uint8_t g,
              uint8_t b)
{
    bool changed = false;

    for(uint8_t i = 0; i < count; i++)
    {
        uint8_t idx = first + i;

        if(idx >= LED_COUNT)
            break;

        if(leds[idx].r != r ||
           leds[idx].g != g ||
           leds[idx].b != b)
        {
            leds[idx].r = r;
            leds[idx].g = g;
            leds[idx].b = b;

            changed = true;
        }
    }

    if(changed)
    {
        dirty = true;
    }
}

void led_show(void)
{
    dirty = true;
}

void led_driver_task(void)
{
    if(!dirty)
        return;

    if(dma_busy)
        return;

    dirty = false;

    build_pwm_buffer();

    dma_busy = true;

    HAL_TIM_PWM_Start_DMA(
        &htim1,
        TIM_CHANNEL_1,
        (uint32_t*)pwm_buffer,
        sizeof(pwm_buffer) / sizeof(pwm_buffer[0]));
}

static void build_pwm_buffer(void)
{
    uint32_t idx = 0;

    for(uint32_t led = 0; led < LED_COUNT; led++)
    {
        uint8_t colors[3] =
        {
            leds[led].g,
            leds[led].r,
            leds[led].b
        };

        for(uint32_t c = 0; c < 3; c++)
        {
            for(int bit = 7; bit >= 0; bit--)
            {
                if(colors[c] & (1 << bit))
                {
                    pwm_buffer[idx++] = SK6805_ONE;
                }
                else
                {
                    pwm_buffer[idx++] = SK6805_ZERO;
                }
            }
        }
    }

    while(idx < (sizeof(pwm_buffer) / sizeof(pwm_buffer[0])))
    {
        pwm_buffer[idx++] = 0;
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance != TIM1)
        return;

    HAL_TIM_PWM_Stop_DMA(
        &htim1,
        TIM_CHANNEL_1);

    dma_busy = false;
}
