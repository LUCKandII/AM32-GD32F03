/**
 * @file    startup_gd32f10x_md.s
 * @brief   GD32F10x medium density startup file for GCC
 */

.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section */
.word _sidata
/* start address for the .data section */
.word _sdata
/* end address for the .data section */
.word _edata
/* start address for the .bss section */
.word _sbss
/* end address for the .bss section */
.word _ebss

/**
 * @brief  This is the code that gets called when the processor first
 *         starts execution following a reset event.
 */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    ldr r0, =_estack
    mov sp, r0

/* Copy the data segment initializers from flash to SRAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata

LoopCopyDataInit:
    ldr r3, [r2]
    adds r2, r2, #4
    str r3, [r0]
    adds r0, r0, #4
    cmp r0, r1
    bcc LoopCopyDataInit

/* Zero fill the bss segment */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
LoopFillZerobss:
    str r2, [r0]
    adds r0, r0, #4
    cmp r0, r1
    bcc LoopFillZerobss

/* Call the clock system initialization function */
    bl SystemInit
/* Call the application entry point */
    bl main
    bx lr
.size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives
 *         an unexpected interrupt.
 */
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
    b Infinite_Loop
    .size Default_Handler, .-Default_Handler

/*
 * Minimal vector table for Cortex-M3
 */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

    /* External Interrupts */
    .word WWDGT_IRQHandler
    .word LVD_IRQHandler
    .word TAMPER_IRQHandler
    .word RTC_IRQHandler
    .word FMC_IRQHandler
    .word RCU_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA0_Channel0_IRQHandler
    .word DMA0_Channel1_IRQHandler
    .word DMA0_Channel2_IRQHandler
    .word DMA0_Channel3_IRQHandler
    .word DMA0_Channel4_IRQHandler
    .word DMA0_Channel5_IRQHandler
    .word DMA0_Channel6_IRQHandler
    .word ADC0_1_IRQHandler
    .word USBD_HP_CAN0_TX_IRQHandler
    .word USBD_LP_CAN0_RX0_IRQHandler
    .word CAN0_RX1_IRQHandler
    .word CAN0_EWMC_IRQHandler
    .word EXTI5_9_IRQHandler
    .word TIMER0_BRK_IRQHandler
    .word TIMER0_UP_IRQHandler
    .word TIMER0_TRG_CMT_IRQHandler
    .word TIMER0_Channel_IRQHandler
    .word TIMER1_IRQHandler
    .word TIMER2_IRQHandler
    .word TIMER3_IRQHandler
    .word I2C0_EV_IRQHandler
    .word I2C0_ER_IRQHandler
    .word I2C1_EV_IRQHandler
    .word I2C1_ER_IRQHandler
    .word SPI0_IRQHandler
    .word SPI1_IRQHandler
    .word USART0_IRQHandler
    .word USART1_IRQHandler
    .word USART2_IRQHandler
    .word EXTI10_15_IRQHandler
    .word RTC_Alarm_IRQHandler
    .word USBD_WKUP_IRQHandler
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word EXMC_IRQHandler

/*
 * Provide weak aliases for each Exception handler to the Default_Handler
 */
.weak NMI_Handler
.thumb_set NMI_Handler,Default_Handler

.weak HardFault_Handler
.thumb_set HardFault_Handler,Default_Handler

.weak MemManage_Handler
.thumb_set MemManage_Handler,Default_Handler

.weak BusFault_Handler
.thumb_set BusFault_Handler,Default_Handler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler,Default_Handler

.weak SVC_Handler
.thumb_set SVC_Handler,Default_Handler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler,Default_Handler

.weak PendSV_Handler
.thumb_set PendSV_Handler,Default_Handler

.weak SysTick_Handler
.thumb_set SysTick_Handler,Default_Handler

.weak WWDGT_IRQHandler
.thumb_set WWDGT_IRQHandler,Default_Handler

.weak LVD_IRQHandler
.thumb_set LVD_IRQHandler,Default_Handler

.weak TAMPER_IRQHandler
.thumb_set TAMPER_IRQHandler,Default_Handler

.weak RTC_IRQHandler
.thumb_set RTC_IRQHandler,Default_Handler

.weak FMC_IRQHandler
.thumb_set FMC_IRQHandler,Default_Handler

.weak RCU_IRQHandler
.thumb_set RCU_IRQHandler,Default_Handler

.weak EXTI0_IRQHandler
.thumb_set EXTI0_IRQHandler,Default_Handler

.weak EXTI1_IRQHandler
.thumb_set EXTI1_IRQHandler,Default_Handler

.weak EXTI2_IRQHandler
.thumb_set EXTI2_IRQHandler,Default_Handler

.weak EXTI3_IRQHandler
.thumb_set EXTI3_IRQHandler,Default_Handler

.weak EXTI4_IRQHandler
.thumb_set EXTI4_IRQHandler,Default_Handler

.weak DMA0_Channel0_IRQHandler
.thumb_set DMA0_Channel0_IRQHandler,Default_Handler

.weak DMA0_Channel1_IRQHandler
.thumb_set DMA0_Channel1_IRQHandler,Default_Handler

.weak DMA0_Channel2_IRQHandler
.thumb_set DMA0_Channel2_IRQHandler,Default_Handler

.weak DMA0_Channel3_IRQHandler
.thumb_set DMA0_Channel3_IRQHandler,Default_Handler

.weak DMA0_Channel4_IRQHandler
.thumb_set DMA0_Channel4_IRQHandler,Default_Handler

.weak DMA0_Channel5_IRQHandler
.thumb_set DMA0_Channel5_IRQHandler,Default_Handler

.weak DMA0_Channel6_IRQHandler
.thumb_set DMA0_Channel6_IRQHandler,Default_Handler

.weak ADC0_1_IRQHandler
.thumb_set ADC0_1_IRQHandler,Default_Handler

.weak USBD_HP_CAN0_TX_IRQHandler
.thumb_set USBD_HP_CAN0_TX_IRQHandler,Default_Handler

.weak USBD_LP_CAN0_RX0_IRQHandler
.thumb_set USBD_LP_CAN0_RX0_IRQHandler,Default_Handler

.weak CAN0_RX1_IRQHandler
.thumb_set CAN0_RX1_IRQHandler,Default_Handler

.weak CAN0_EWMC_IRQHandler
.thumb_set CAN0_EWMC_IRQHandler,Default_Handler

.weak EXTI5_9_IRQHandler
.thumb_set EXTI5_9_IRQHandler,Default_Handler

.weak TIMER0_BRK_IRQHandler
.thumb_set TIMER0_BRK_IRQHandler,Default_Handler

.weak TIMER0_UP_IRQHandler
.thumb_set TIMER0_UP_IRQHandler,Default_Handler

.weak TIMER0_TRG_CMT_IRQHandler
.thumb_set TIMER0_TRG_CMT_IRQHandler,Default_Handler

.weak TIMER0_Channel_IRQHandler
.thumb_set TIMER0_Channel_IRQHandler,Default_Handler

.weak TIMER1_IRQHandler
.thumb_set TIMER1_IRQHandler,Default_Handler

.weak TIMER2_IRQHandler
.thumb_set TIMER2_IRQHandler,Default_Handler

.weak TIMER3_IRQHandler
.thumb_set TIMER3_IRQHandler,Default_Handler

.weak I2C0_EV_IRQHandler
.thumb_set I2C0_EV_IRQHandler,Default_Handler

.weak I2C0_ER_IRQHandler
.thumb_set I2C0_ER_IRQHandler,Default_Handler

.weak I2C1_EV_IRQHandler
.thumb_set I2C1_EV_IRQHandler,Default_Handler

.weak I2C1_ER_IRQHandler
.thumb_set I2C1_ER_IRQHandler,Default_Handler

.weak SPI0_IRQHandler
.thumb_set SPI0_IRQHandler,Default_Handler

.weak SPI1_IRQHandler
.thumb_set SPI1_IRQHandler,Default_Handler

.weak USART0_IRQHandler
.thumb_set USART0_IRQHandler,Default_Handler

.weak USART1_IRQHandler
.thumb_set USART1_IRQHandler,Default_Handler

.weak USART2_IRQHandler
.thumb_set USART2_IRQHandler,Default_Handler

.weak EXTI10_15_IRQHandler
.thumb_set EXTI10_15_IRQHandler,Default_Handler

.weak RTC_Alarm_IRQHandler
.thumb_set RTC_Alarm_IRQHandler,Default_Handler

.weak USBD_WKUP_IRQHandler
.thumb_set USBD_WKUP_IRQHandler,Default_Handler

.weak EXMC_IRQHandler
.thumb_set EXMC_IRQHandler,Default_Handler
