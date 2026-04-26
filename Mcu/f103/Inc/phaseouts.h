/**
 * phaseouts.h
 *
 * Phase output control for motor commutation
 */

#ifndef PHASEOUTS_H
#define PHASEOUTS_H

#include "main.h"

void phaseouts_init(void);
void comStep(int newStep);
void allOff(void);
void allpwm(void);
void fullBrake(void);
void proportionalBrake(void);

#endif /* PHASEOUTS_H */
