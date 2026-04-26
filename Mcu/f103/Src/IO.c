/**
 * IO.c
 *
 * Input/Output configuration for DShot input capture
 */

#include "main.h"
#include "IO.h"
#include "peripherals.h"
#include "common.h"
#include "targets.h"

char ic_timer_prescaler = 0;
uint32_t dma_buffer[64] = {0};
volatile char out_put = 0;
uint8_t buffer_padding = 0;
uint8_t buffer_size = 32;
uint16_t change_time = 0;

void IO_Init(void)
{
    /* DShot input is configured in UN_TIM_Init() which is called from enableCorePeripherals() */
}

uint8_t getInputPinState(void)
{
    return (uint8_t)((GPIO_ISTAT(GPIOA) & INPUT_PIN) != 0);
}

void receiveDshotDma(void)
{
    /* DMA transfer for DShot is handled by DMA controller */
}

void sendDshotDma(void)
{
    /* Not used for DShot, only for bidirectional DShot TX */
}

void setInputPolarityRising(void)
{
    /* Configure input capture to trigger on rising edge */
    TIMER_CHCTL2(TIMER2) &= ~(uint32_t)(TIMER_CHCTL2_CH0NP);
}

void setInputPullDown(void)
{
    /* Configure pull-down on input pin
     * Note: For DShot, we typically don't use pull resistors
     * This is a stub for compatibility
     */
}

void setInputPullUp(void)
{
    /* Configure pull-up on input pin - not typically used for DShot */
}

void setInputPullNone(void)
{
    /* No pull resistors on input pin */
}

void enableHalfTransferInt(void)
{
    /* Enable DMA half-transfer interrupt */
    DMA_CHCTL(DMA0, DMA_CH2) |= DMA_CHXCTL_HTFIE;
}

void disableHalfTransferInt(void)
{
    /* Disable DMA half-transfer interrupt */
    DMA_CHCTL(DMA0, DMA_CH2) &= ~(uint32_t)DMA_CHXCTL_HTFIE;
}
