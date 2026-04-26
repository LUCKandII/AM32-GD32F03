/**
 * peripherals.c
 *
 * Peripheral initialization for GD32F103
 */

#include "peripherals.h"
#include "ADC.h"
#include "common.h"
#include "functions.h"
#include "serial_telemetry.h"
#include "targets.h"

extern uint32_t SystemCoreClock;

/* DMA buffer for DShot input */
extern uint32_t dma_buffer[64];

void initCorePeripherals(void)
{
    MX_GPIO_Init();
    MX_DMA_Init();
    TIM0_Init();
    TIMER5_Init();
    TIMER4_Init();
    TIMER6_Init();
    /* TIMER13 doesn't have dedicated IRQ in GD32F10x, skip for now */
    COMP_Init();
#ifdef USE_RGB_LED
    LED_GPIO_init();
#endif
#ifdef USE_SERIAL_TELEMETRY
    telem_UART_Init();
#endif
}

void COMP_Init(void)
{
    /* Comparator initialization for BEMF detection
     * GD32F10x has built-in comparators but they need direct register access
     * For now, use stub - will be implemented with proper register access
     */
}

void MX_IWDG_Init(void)
{
    fwdgt_config(4000, FWDGT_PSC_DIV16);
    fwdgt_enable();
}

void reloadWatchDogCounter(void)
{
    fwdgt_counter_reload();
}

void TIM0_Init(void)
{
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_break_parameter_struct timer_breakpara;

    /* Enable TIMER0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);

    /* Deinit TIMER0 */
    timer_deinit(TIMER0);

    /* Initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);

    /* TIMER0 configuration: 96MHz, complementary PWM */
    timer_initpara.prescaler = 0;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = TIM1_AUTORELOAD;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* Initialize channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);

    /* Configure CH0/CH0N, CH1/CH1N and CH2/CH2N in timing mode */
    timer_ocinitpara.outputstate = TIMER_CCX_DISABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocinitpara);

    /* Configure TIMER channel 0 - PWM mode 0 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);

    /* Configure TIMER channel 1 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_ENABLE);

    /* Configure TIMER channel 2 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_ENABLE);

    /* Configure break and deadtime */
    timer_break_struct_para_init(&timer_breakpara);
    timer_breakpara.runoffstate = TIMER_ROS_STATE_DISABLE;
    timer_breakpara.ideloffstate = TIMER_IOS_STATE_DISABLE;
    timer_breakpara.deadtime = DEAD_TIME;
    timer_breakpara.breakpolarity = TIMER_BREAK_POLARITY_HIGH;
    timer_breakpara.outputautostate = TIMER_OUTAUTO_DISABLE;
    timer_breakpara.protectmode = TIMER_CCHP_PROT_OFF;
    timer_breakpara.breakstate = TIMER_BREAK_DISABLE;
    timer_break_config(TIMER0, &timer_breakpara);

    /* Disable interrupts, enable auto-reload shadow */
    timer_interrupt_disable(TIMER0, TIMER_INT_CMT);
    timer_interrupt_disable(TIMER0, TIMER_INT_BRK);
    timer_auto_reload_shadow_enable(TIMER0);

    /* Enable GPIO clocks */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* Configure PA8/PA9/PA10 as alternate function push-pull (TIMER0 CH0/CH1/CH2) */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,
              GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

    /* Configure PB13/PB14/PB15 as alternate function push-pull (TIMER0 CH0N/CH1N/CH2N) */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,
              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}

void TIMER4_Init(void)  /* INTERVAL_TIMER */
{
    rcu_periph_clock_enable(RCU_TIMER4);
    TIMER_CAR(TIMER4) = 0xFFFF;
    TIMER_PSC(TIMER4) = 71;
    timer_auto_reload_shadow_enable(TIMER4);
}

void TIMER5_Init(void)  /* UTILITY_TIMER */
{
    rcu_periph_clock_enable(RCU_TIMER5);
    TIMER_CAR(TIMER5) = 0xFFFF;
    TIMER_PSC(TIMER5) = 35;
}

void TIMER6_Init(void)  /* TEN_KHZ_TIMER */
{
    rcu_periph_clock_enable(RCU_TIMER6);
    TIMER_CAR(TIMER6) = 1000000 / LOOP_FREQUENCY_HZ;
    TIMER_PSC(TIMER6) = 71;
    NVIC_SetPriority(TIMER6_IRQn, 3);
    NVIC_EnableIRQ(TIMER6_IRQn);
    timer_enable(TIMER6);
}

/* Note: TIMER13 doesn't exist in GD32F10x_HD, use TIMER7 instead for commutation */
void TIMER13_Init(void)  /* COM_TIMER */
{
    /* TIMER7 is used instead of TIMER13 since TIMER13 doesn't exist in HD */
    rcu_periph_clock_enable(RCU_TIMER7);
    TIMER_CAR(TIMER7) = 1000000 / LOOP_FREQUENCY_HZ;
    TIMER_PSC(TIMER7) = 71;
    NVIC_SetPriority(TIMER7_TRG_CMT_IRQn, 3);
    NVIC_EnableIRQ(TIMER7_TRG_CMT_IRQn);
    timer_enable(TIMER7);
}

void TIMER2_Init(void)
{
    /* TIMER2 is used for DShot input capture */
    rcu_periph_clock_enable(RCU_TIMER2);
    TIMER_CAR(TIMER2) = 0xFFFF;
    TIMER_PSC(TIMER2) = 0;  /* Full speed for input capture */
    timer_auto_reload_shadow_disable(TIMER2);
}

void MX_DMA_Init(void)
{
    rcu_periph_clock_enable(RCU_DMA0);
    NVIC_SetPriority(DMA0_Channel2_IRQn, 1);
    NVIC_EnableIRQ(DMA0_Channel2_IRQn);
}

void MX_GPIO_Init(void)
{
    /* Enable all GPIO clocks */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);

    /* Configure LED pin (PB3) as output */
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_bit_reset(GPIOB, GPIO_PIN_3);  /* LED off initially */

    /* Configure current sense ADC pins (PA1, PA2, PA3) as analog input */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ,
              GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    /* Configure voltage and FOC ADC pins (PA4, PA5, PA6, PA7) as analog input */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ,
              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    /* Configure temperature sensor ADC pin (PB0) as analog input */
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}

void UN_TIM_Init(void)
{
    timer_ic_parameter_struct timer_icinitpara;

    /* Enable clocks */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_TIMER2);
    rcu_periph_clock_enable(RCU_DMA0);

    /* Configure PA0 as alternate function push-pull (TIMER2 CH0) */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    /* Configure DMA for TIMER2 CH0 input capture */
    dma_parameter_struct dma_initpara;
    dma_deinit(DMA0, DMA_CH2);

    dma_initpara.periph_addr = (uint32_t)(&TIMER_CH0CV(TIMER2));
    dma_initpara.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_initpara.memory_addr = (uint32_t)dma_buffer;
    dma_initpara.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_initpara.number = 64;
    dma_initpara.priority = DMA_PRIORITY_LOW;
    dma_initpara.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init(DMA0, DMA_CH2, &dma_initpara);

    /* Configure TIMER2 for PWM input capture */
    timer_channel_input_struct_para_init(&timer_icinitpara);
    timer_icinitpara.icpolarity = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter = 0x0;
    timer_input_pwm_capture_config(TIMER2, TIMER_CH_0, &timer_icinitpara);

    /* Enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH2);

    /* Enable TIMER2 */
    timer_enable(TIMER2);
}

#ifdef USE_RGB_LED
void LED_GPIO_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
}

void setIndividualRGBLed(uint8_t red, uint8_t green, uint8_t blue)
{
    if (red > 0)
        gpio_bit_set(GPIOB, GPIO_PIN_3);
    else
        gpio_bit_reset(GPIOB, GPIO_PIN_3);
}
#endif

void setPWMCompare1(uint16_t compareone)
{
    TIMER_CH0CV(TIMER0) = compareone;
}

void setPWMCompare2(uint16_t comparetwo)
{
    TIMER_CH1CV(TIMER0) = comparetwo;
}

void setPWMCompare3(uint16_t comparethree)
{
    TIMER_CH2CV(TIMER0) = comparethree;
}

void generatePwmTimerEvent(void)
{
    timer_event_software_generate(TIMER0, TIMER_EVENT_SRC_UPG);
}

void resetInputCaptureTimer(void)
{
    TIMER_PSC(TIMER2) = 0;
    TIMER_CNT(TIMER2) = 0;
}

void initAfterJump(void)
{
    __enable_irq();
    /* Configure Flash wait states for 96MHz */
    fmc_wscnt_set(WS_WSCNT_2);
}

void enableCorePeripherals(void)
{
    /* Enable TIMER0 PWM outputs */
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_ENABLE);

    /* Enable TIMER0 counter */
    TIMER_CTL0(TIMER0) |= (uint32_t)TIMER_CTL0_CEN;
    timer_primary_output_config(TIMER0, ENABLE);

#ifndef BRUSHED_MODE
    /* Enable commutation timer - TIMER13 (polling mode, no IRQ) */
    TIMER_CTL0(COM_TIMER) |= (uint32_t)TIMER_CTL0_CEN;
    timer_event_software_generate(COM_TIMER, TIMER_EVENT_SRC_UPG);
    TIMER_DMAINTEN(COM_TIMER) &= (~(uint32_t)TIMER_INT_UP);
#endif

    /* Enable utility and interval timers */
    TIMER_CTL0(UTILITY_TIMER) |= (uint32_t)TIMER_CTL0_CEN;
    TIMER_CTL0(INTERVAL_TIMER) |= (uint32_t)TIMER_CTL0_CEN;
    timer_event_software_generate(INTERVAL_TIMER, TIMER_EVENT_SRC_UPG);

    /* Enable 10kHz timer */
    TIMER_CTL0(TEN_KHZ_TIMER) |= (uint32_t)TIMER_CTL0_CEN;
    timer_event_software_generate(TEN_KHZ_TIMER, TIMER_EVENT_SRC_UPG);
    TIMER_DMAINTEN(TEN_KHZ_TIMER) |= (uint32_t)TIMER_INT_UP;

#ifdef USE_ADC
    ADC_Init();
#endif

    delayMicros(1000);

#ifdef USE_ADC_INPUT
#else
    /* Enable input capture */
    timer_channel_output_state_config(TIMER2, TIMER_CH_0, TIMER_CCX_ENABLE);
    TIMER_CTL0(TIMER2) |= (uint32_t)TIMER_CTL0_CEN;
#endif

    UN_TIM_Init();
}
