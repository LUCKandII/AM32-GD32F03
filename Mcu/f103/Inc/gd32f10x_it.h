/**
 * @file    gd32f10x_it.h
 * @brief   Interrupt handler prototypes
 */

#ifndef GD32F10x_IT_H
#define GD32F10x_IT_H

#include "main.h"

/* Interrupt handler declarations */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void WWDGT_IRQHandler(void);
void LVD_IRQHandler(void);
void TAMPER_IRQHandler(void);
void RTC_IRQHandler(void);
void FMC_IRQHandler(void);
void RCU_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void DMA0_Channel0_IRQHandler(void);
void DMA0_Channel1_IRQHandler(void);
void DMA0_Channel2_IRQHandler(void);
void DMA0_Channel3_IRQHandler(void);
void DMA0_Channel4_IRQHandler(void);
void DMA0_Channel5_IRQHandler(void);
void DMA0_Channel6_IRQHandler(void);
void ADC0_1_IRQHandler(void);
void USBD_HP_CAN0_TX_IRQHandler(void);
void USBD_LP_CAN0_RX0_IRQHandler(void);
void CAN0_RX1_IRQHandler(void);
void CAN0_EWMC_IRQHandler(void);
void EXTI5_9_IRQHandler(void);
void TIMER0_BRK_IRQHandler(void);
void TIMER0_UP_IRQHandler(void);
void TIMER0_TRG_CMT_IRQHandler(void);
void TIMER0_Channel_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);
void I2C0_EV_IRQHandler(void);
void I2C0_ER_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
void SPI0_IRQHandler(void);
void SPI1_IRQHandler(void);
void USART0_IRQHandler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void EXTI10_15_IRQHandler(void);
void RTC_Alarm_IRQHandler(void);
void USBD_WKUP_IRQHandler(void);
void EXMC_IRQHandler(void);

#endif /* GD32F10x_IT_H */
