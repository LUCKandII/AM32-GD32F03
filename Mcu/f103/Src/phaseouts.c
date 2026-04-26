/**
 * phaseouts.c
 *
 * Phase output control for motor commutation
 */

#include "phaseouts.h"
#include "peripherals.h"
#include "targets.h"

void phaseouts_init(void)
{
    /* Phase outputs are configured in TIM0_Init() and enableCorePeripherals() */
}

/* Phase output control functions - called from main control loop */
void set_phase_a_high(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
}

void set_phase_a_low(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_ENABLE);
}

void set_phase_b_high(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
}

void set_phase_b_low(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_ENABLE);
}

void set_phase_c_high(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);
}

void set_phase_c_low(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_ENABLE);
}

void set_all_phases_low(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);
}

void phaseAPWM(void) { set_phase_a_high(); }
void phaseAFLOAT(void) { set_all_phases_low(); }
void phaseALOW(void) { set_phase_a_low(); }

void phaseBPWM(void) { set_phase_b_high(); }
void phaseBFLOAT(void) { set_all_phases_low(); }
void phaseBLOW(void) { set_phase_b_low(); }

void phaseCPWM(void) { set_phase_c_high(); }
void phaseCFLOAT(void) { set_all_phases_low(); }
void phaseCLOW(void) { set_phase_c_low(); }

void allOff(void)
{
    phaseAFLOAT();
    phaseBFLOAT();
    phaseCFLOAT();
}

void allpwm(void)
{
    phaseAPWM();
    phaseBPWM();
    phaseCPWM();
}

void fullBrake(void)
{
    phaseALOW();
    phaseBLOW();
    phaseCLOW();
}

void proportionalBrake(void)
{
    /* For proportional brake, we'd need to switch low sides to PWM mode */
    /* This is a simplified version - full braking */
    fullBrake();
}

void comStep(int newStep)
{
    switch (newStep) {
    case 1: // A-B (C float)
        phaseCFLOAT();
        phaseBLOW();
        phaseAPWM();
        break;
    case 2: // C-B (A float)
        phaseAFLOAT();
        phaseBLOW();
        phaseCPWM();
        break;
    case 3: // C-A (B float)
        phaseBFLOAT();
        phaseALOW();
        phaseCPWM();
        break;
    case 4: // B-A (C float)
        phaseCFLOAT();
        phaseALOW();
        phaseBPWM();
        break;
    case 5: // B-C (A float)
        phaseAFLOAT();
        phaseCLOW();
        phaseBPWM();
        break;
    case 6: // A-C (B float)
        phaseBFLOAT();
        phaseCLOW();
        phaseAPWM();
        break;
    }
}
