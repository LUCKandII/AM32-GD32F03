/**
 * systick.h
 *
 * SysTick configuration for delay functions
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include "main.h"

void systick_config(void);
void delay_1ms(uint32_t time);

#endif /* SYSTICK_H */
