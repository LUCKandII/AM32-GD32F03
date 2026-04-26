/**
 * @file    systick.c
 * @brief   SysTick configuration for delay functions
 */

#include "systick.h"
#include "gd32f10x.h"

/**
 * @brief  Configure a SysTick
 * @param  ticks: number of ticks between two interrupts
 * @retval None
 */
void systick_config(void)
{
    /* Setup SysTick timer for 1ms interrupts */
    if (SysTick_Config(SystemCoreClock / 1000)) {
        /* Capture error */
        while (1) {
        }
    }

    /* Configure SysTick priority */
    NVIC_SetPriority(SysTick_IRQn, 0xFF);
}

/**
 * @brief  Delay function in milliseconds
 * @param  time: delay duration in milliseconds
 */
void delay_1ms(uint32_t time)
{
    uint32_t i;
    for (i = 0; i < time; i++) {
        /* 1ms delay using DWT cycle counter if available */
        uint32_t tickstart = SysTick->VAL;
        uint32_t wait = SystemCoreClock / 1000;

        while ((SysTick->VAL - tickstart) < wait) {
            /* Wait */
        }
    }
}
