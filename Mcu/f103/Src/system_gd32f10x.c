/**
 * system_gd32f10x.c
 *
 * System clock initialization for GD32F103 at 96MHz
 * Uses 8MHz HXTAL -> PLL * 12 = 96MHz
 */

#include "main.h"

uint32_t SystemCoreClock = 96000000;

void SystemInit(void)
{
    /* Reset registers to default state */
    rcu_deinit();

    /* Enable HXTAL (8MHz external crystal) */
    rcu_osci_on(RCU_HXTAL);
    /* Wait for HXTAL stable */
    while (SUCCESS != rcu_osci_stab_wait(RCU_HXTAL)) {
    }

    /* Configure PLL:
     * PLL = HXTAL * 12 = 8MHz * 12 = 96MHz
     * PCLK1 = PLL / 2 = 48MHz (max 60MHz)
     * PCLK2 = PLL = 96MHz (max 120MHz)
     */
    rcu_pll_config(RCU_PLLSRC_HXTAL, RCU_PLL_MUL12);

    /* Enable PLL */
    rcu_osci_on(RCU_PLL_CK);
    /* Wait for PLL stable */
    while (SUCCESS != rcu_osci_stab_wait(RCU_PLL_CK)) {
    }

    /* Configure AHB, APB1, APB2 prescalers */
    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);   /* HCLK = 96MHz */
    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV2); /* PCLK1 = 48MHz */
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1); /* PCLK2 = 96MHz */

    /* Select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
    while (RCU_CKSYSSRC_PLL != rcu_system_clock_source_get()) {
    }

    /* Update SystemCoreClock */
    SystemCoreClock = 96000000;

    /* Configure Flash wait states for 96MHz:
     * 2 wait states for 48MHz < HCLK <= 96MHz
     */
    fmc_wscnt_set(WS_WSCNT_2);

    /* Flash prefetch is enabled by default in GD32F10x */
}
