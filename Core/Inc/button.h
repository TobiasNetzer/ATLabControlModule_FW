/*
 * button.h
 *
 *  Created on: 04.06.2026
 *      Author: Tobias
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    BUTTON_START = 0,
    BUTTON_STOP  = 1

} button_id_t;

typedef enum
{
    BUTTON_EVENT_PRESS   = 1,
    BUTTON_EVENT_RELEASE = 2

} button_event_t;

void button_init(void);
void button_task(void);

#endif /* INC_BUTTON_H_ */
