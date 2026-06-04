/*
 * button.c
 *
 *  Created on: 04.06.2026
 *      Author: Tobias
 */
#include "button.h"

#include "main.h"
#include "tusb.h"

#define BUTTON_DEBOUNCE_MS 20

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;

    bool stable_state;
    bool last_sample;

    uint32_t last_change;

} button_state_t;

static button_state_t buttons[2];

static bool read_button(button_state_t* btn);
static void send_button_event(button_id_t id,
                              button_event_t event);

void button_init(void)
{
    buttons[BUTTON_START].port = START_BTN_GPIO_Port;
    buttons[BUTTON_START].pin  = START_BTN_Pin;

    buttons[BUTTON_STOP].port = STOP_BTN_GPIO_Port;
    buttons[BUTTON_STOP].pin  = STOP_BTN_Pin;

    for(uint32_t i = 0; i < 2; i++)
    {
        bool state = read_button(&buttons[i]);

        buttons[i].stable_state = state;
        buttons[i].last_sample  = state;
        buttons[i].last_change  = HAL_GetTick();
    }
}

void button_task(void)
{
    uint32_t now = HAL_GetTick();

    for(uint32_t i = 0; i < 2; i++)
    {
        button_state_t* btn = &buttons[i];

        bool sample = read_button(btn);

        if(sample != btn->last_sample)
        {
            btn->last_sample = sample;
            btn->last_change = now;
        }

        if((now - btn->last_change) >= BUTTON_DEBOUNCE_MS)
        {
            if(sample != btn->stable_state)
            {
                btn->stable_state = sample;

                if(sample)
                {
                    send_button_event(
                        (button_id_t)i,
                        BUTTON_EVENT_PRESS);
                }
                else
                {
                	return;
                	// don't send release event
                    send_button_event(
                        (button_id_t)i,
                        BUTTON_EVENT_RELEASE);
                }
            }
        }
    }
}

static bool read_button(button_state_t* btn)
{
    return HAL_GPIO_ReadPin(
               btn->port,
               btn->pin) == GPIO_PIN_RESET;
}

static void send_button_event(button_id_t id,
                              button_event_t event)
{
    if(!tud_hid_ready())
        return;

    uint8_t report[2];

    report[0] = (uint8_t)event;
    report[1] = (uint8_t)id;

    tud_hid_report(
        1,
        report,
        sizeof(report));
}