/**
 * @file    system_gd32f10x.h
 * @brief   System clock configuration header
 */

#ifndef SYSTEM_GD32F10X_H
#define SYSTEM_GD32F10X_H

#include <stdint.h>

/* System clock frequency */
extern uint32_t SystemCoreClock;

/* Function declarations */
void SystemInit(void);
void SystemCoreClockUpdate(void);

#endif /* SYSTEM_GD32F10X_H */
