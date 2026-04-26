/**
 * comparator.c
 *
 * Comparator configuration for BEMF detection
 *
 * GD32F103 has built-in comparators but no standard peripheral library driver.
 * Requires direct register access (similar to E230).
 */

#include "main.h"
#include "comparator.h"
#include "peripherals.h"
#include "targets.h"

static uint8_t current_comp_input = 0;

/* External variable from main.c - current commutation step */
extern volatile uint8_t step;
extern volatile uint8_t rising;

void comparator_init(void)
{
    /* Enable clocks */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_CMPEN);

    /* Configure PB1, PB2, PB10 as analog input for comparator */
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ,
              GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10);

    /* Initial comparator configuration - Phase A (PB1/COMP1) */
    CMP->CS = CMP_CS_EN | CMP_SPEED_HIGH | (PHASE_A_COMP & CMP_CS_MSEL);

    /* Configure EXTI for comparator interrupt */
    /* Comparator output connected to EXTI line through sync block */
    exti_init(EXTI_3, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_3);

    /* Enable comparator interrupt in NVIC */
    nvic_irq_enable(COMPARATOR_IRQ, 0, 0);
}

uint8_t getCompOutputLevel(void)
{
    /* Read comparator output level from CMP_CS register */
    return (CMP->CS & CMP_CS_OUTPUT) ? CMP_OUTPUT_HIGH : CMP_OUTPUT_LOW;
}

void maskPhaseInterrupts(void)
{
    /* Disable EXTI interrupt */
    EXTI_INTEN &= ~(uint32_t)EXTI_3;
    /* Clear pending flag */
    EXTI_PD = (uint32_t)EXTI_3;
}

void enableCompInterrupts(void)
{
    /* Enable EXTI interrupt */
    EXTI_INTEN |= (uint32_t)EXTI_3;
}

void changeCompInput(void)
{
    /* Select appropriate comparator input based on current commutation step */
    uint32_t comp_config = CMP_CS_EN | CMP_SPEED_HIGH;

    if (step == 1 || step == 4) {
        /* Phase C floating - use COMP3 on PB10 */
        comp_config |= (PHASE_C_COMP & CMP_CS_MSEL);
    } else if (step == 2 || step == 5) {
        /* Phase A floating - use COMP1 on PB1 */
        comp_config |= (PHASE_A_COMP & CMP_CS_MSEL);
    } else if (step == 3 || step == 6) {
        /* Phase B floating - use COMP2 on PB2 */
        comp_config |= (PHASE_B_COMP & CMP_CS_MSEL);
    }

    /* Configure edge trigger based on motor direction */
    if (rising) {
        /* Rising edge - enable falling trigger for BEMF */
        EXTI_RTEN &= ~(uint32_t)EXTI_3;
        EXTI_FTEN |= (uint32_t)EXTI_3;
    } else {
        /* Falling edge - enable rising trigger for BEMF */
        EXTI_RTEN |= (uint32_t)EXTI_3;
        EXTI_FTEN &= ~(uint32_t)EXTI_3;
    }

    CMP->CS = comp_config;
    current_comp_input = step;
}
