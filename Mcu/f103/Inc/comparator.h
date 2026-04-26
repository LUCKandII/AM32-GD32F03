/**
 * comparator.h
 *
 * Comparator configuration for BEMF detection
 *
 * GD32F103 has built-in comparators but no standard peripheral library driver.
 * This file provides direct register access definitions.
 */

#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "main.h"

/* Comparator base address for GD32F103 */
#define CMP_BASE                0x4001C000
#define CMP                     ((CMP_TypeDef *) CMP_BASE)

typedef struct {
    __IO uint32_t CS;    /*!< comparator control/status register */
    __IO uint32_t CR;    /*!< comparator control register */
} CMP_TypeDef;

/* CMP_CS register bits */
#define CMP_CS_EN              BIT(0)        /* comparator enable */
#define CMP_CS_SWITCH          BIT(1)        /* comparator switch */
#define CMP_CS_SPEED           BITS(2,3)     /* speed mode */
#define CMP_CS_MSEL            BITS(4,6)     /* minus input selection */
#define CMP_CS_OSEL            BITS(8,10)    /* output selection */
#define CMP_CS_POL             BIT(11)       /* output polarity */
#define CMP_CS_HYST           BITS(12,13)   /* hysteresis */
#define CMP_CS_OUTPUT          BIT(14)       /* comparator output level */
#define CMP_CS_LOCK            BIT(15)       /* lock */

/* Output levels */
#define CMP_OUTPUT_HIGH        1
#define CMP_OUTPUT_LOW         0

/* Speed modes */
#define CMP_SPEED_HIGH         0
#define CMP_SPEED_MEDIUM       1
#define CMP_SPEED_LOW          2
#define CMP_SPEED_VERYLOW     3

/* Hysteresis */
#define CMP_HYST_NO            0
#define CMP_HYST_3MV           1
#define CMP_HYST_6MV           2
#define CMP_HYST_9MV           3

/* Comparator clock enable - RCU_APB1EN bit 31 for GD32F103 */
#ifndef RCU_CMPEN
#define RCU_CMPEN              RCU_REGIDX_BIT(APB1EN_REG_OFFSET, 31U)
#endif

/* Function declarations */
void comparator_init(void);
void maskPhaseInterrupts(void);
void changeCompInput(void);
void enableCompInterrupts(void);
uint8_t getCompOutputLevel(void);

#endif /* COMPARATOR_H */
