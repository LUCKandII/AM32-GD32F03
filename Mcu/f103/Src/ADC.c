/**
 * ADC.c
 *
 * ADC configuration for current and voltage sensing
 */

#include "main.h"
#include "ADC.h"
#include "common.h"
#include "functions.h"
#include "targets.h"

void ADC_Init(void)
{
    /* Enable ADC0 clock */
    rcu_periph_clock_enable(RCU_ADC0);

    /* Configure ADC - use independent mode */
    adc_mode_config(ADC_MODE_FREE);

    /* Configure ADC continuous conversion mode */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);

    /* Configure data alignment */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* Configure channel length - 4 channels in regular group */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 8);

    /* Configure regular channel - PA1 (ADC channel 1) */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_1POINT5);

    /* Configure regular channel - PA2 (ADC channel 2) */
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_2, ADC_SAMPLETIME_1POINT5);

    /* Configure regular channel - PA3 (ADC channel 3) */
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_3, ADC_SAMPLETIME_1POINT5);

    /* Configure regular channel - PA4 (ADC channel 4) */
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_4, ADC_SAMPLETIME_1POINT5);

    /* Configure regular channel - PA5 (ADC channel 5) - Phase voltage V_B (FOC预留) */
    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_5, ADC_SAMPLETIME_1POINT5);

    /* Configure regular channel - PA6 (ADC channel 6) - Phase voltage V_C (FOC预留) */
    adc_regular_channel_config(ADC0, 5, ADC_CHANNEL_6, ADC_SAMPLETIME_1POINT5);

    /* Configure regular channel - PA7 (ADC channel 7) - Bus voltage V_BUS */
    adc_regular_channel_config(ADC0, 6, ADC_CHANNEL_7, ADC_SAMPLETIME_1POINT5);

    /* Configure regular channel - PB0 (ADC channel 8) - Temperature sensor */
    adc_regular_channel_config(ADC0, 7, ADC_CHANNEL_8, ADC_SAMPLETIME_1POINT5);

    /* Enable ADC */
    adc_enable(ADC0);

    /* Wait for ADC stabilization */
    delayMicros(10);

    /* Calibrate ADC */
    adc_calibration_enable(ADC0);

    /* Start ADC conversion */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

void DMA_Init(void)
{
    /* DMA for ADC is configured separately if needed */
}

uint16_t getCurrentAdcValue(uint8_t channel)
{
    /* Read ADC value - simplified stub */
    (void)channel;
    return 0;
}
